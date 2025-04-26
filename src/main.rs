pub mod callers;

pub mod db {

    use sqlx::postgres::PgPoolOptions;
    use std::env;

    pub mod connection_settings {
        pub const MAXCONN: u32 = 10;
    }

    pub mod keys {
        pub const DBURL: &str = "DATABASE_URL";

        pub mod error {
            pub const ERROR: &str = "DATABASE_URL must be set in .env";
        }
    }

    pub async fn create_pool() -> Result<sqlx::PgPool, sqlx::Error> {
        let database_url = get_db_url().await;
        println!("Database url: {:?}", database_url);

        PgPoolOptions::new()
            .max_connections(connection_settings::MAXCONN)
            .connect(&database_url)
            .await
    }

    async fn get_db_url() -> String {
        dotenvy::dotenv().ok();
        env::var(keys::DBURL).expect(keys::error::ERROR)
    }

    pub async fn migrations(pool: &sqlx::PgPool) {
        // Run migrations using the sqlx::migrate! macro
        // Assumes your migrations are in a ./migrations folder relative to Cargo.toml
        sqlx::migrate!("./migrations")
            .run(pool)
            .await
            .expect("Failed to run migrations");
    }
}

#[tokio::main]
async fn main() {
    // initialize tracing
    tracing_subscriber::fmt::init();

    let pool = db::create_pool().await.expect("Failed to create pool");
    db::migrations(&pool).await;

    // build our application with a route
    let app = init::app().await;

    let listener = tokio::net::TcpListener::bind(get_full()).await.unwrap();
    axum::serve(listener, app).await.unwrap();
}

pub mod init {
    use axum::routing::{get, post};
    use std::time::Duration;
    use tower_http::timeout::TimeoutLayer;

    pub async fn routes() -> axum::Router {
        axum::Router::new()
            .route(crate::ROOT, get(crate::root))
            .route(
                crate::callers::endpoints::QUEUESONG,
                post(crate::callers::song::endpoint::queue_song),
            )
            .route(
                crate::callers::endpoints::NEXTQUEUESONG,
                get(crate::callers::song::endpoint::fetch_queue_song),
            )
            .route(
                crate::callers::endpoints::QUEUEMETADATA,
                post(crate::callers::metadata::endpoint::queue_metadata),
            )
    }

    pub async fn app() -> axum::Router {
        let pool = crate::db::create_pool()
            .await
            .expect("Failed to create pool");
        crate::db::migrations(&pool).await;

        routes()
            .await
            .layer(axum::Extension(pool))
            .layer(axum::extract::DefaultBodyLimit::max(1024 * 1024 * 1024))
            .layer(TimeoutLayer::new(Duration::from_secs(300)))
    }
}

fn get_full() -> String {
    get_address() + ":" + &get_port()
}
fn get_address() -> String {
    String::from("0.0.0.0")
}

fn get_port() -> String {
    String::from("3000")
}

pub const ROOT: &str = "/";
// basic handler that responds with a static string
pub async fn root() -> &'static str {
    "Hello, World!"
}

#[cfg(test)]
mod tests {
    use crate::db;

    use common_multipart_rfc7578::client::multipart::{
        Body as MultipartBody, Form as MultipartForm,
    };
    use std::{time::Duration, usize};
    use tower::ServiceExt;
    use tower_http::timeout::TimeoutLayer;

    mod db_mgr {
        use std::str::FromStr;

        use crate::db;

        pub const LIMIT: usize = 6;

        pub async fn get_pool() -> Result<sqlx::PgPool, sqlx::Error> {
            dotenvy::dotenv().ok();
            let tm_db_url = std::env::var(db::keys::DBURL).expect("DATABASE_URL must be present");
            let tm_options = sqlx::postgres::PgConnectOptions::from_str(&tm_db_url).unwrap();
            sqlx::PgPool::connect_with(tm_options).await
        }

        pub async fn generate_db_name() -> String {
            let db_name =
                get_database_name().unwrap() + &"_" + &uuid::Uuid::new_v4().to_string()[..LIMIT];
            db_name
        }

        pub async fn connect_to_db(db_name: &str) -> Result<sqlx::PgPool, sqlx::Error> {
            dotenvy::dotenv().ok();
            let db_url =
                std::env::var(db::keys::DBURL).expect("DATABASE_URL must be set for tests");
            let options = sqlx::postgres::PgConnectOptions::from_str(&db_url)?.database(db_name);
            sqlx::PgPool::connect_with(options).await
        }

        pub async fn create_database(
            template_pool: &sqlx::PgPool,
            db_name: &str,
        ) -> Result<(), sqlx::Error> {
            let create_query = format!("CREATE DATABASE {}", db_name);
            match sqlx::query(&create_query).execute(template_pool).await {
                Ok(_) => Ok(()),
                Err(e) => Err(e),
            }
        }

        // Function to drop a database
        pub async fn drop_database(
            template_pool: &sqlx::PgPool,
            db_name: &str,
        ) -> Result<(), sqlx::Error> {
            let drop_query = format!("DROP DATABASE IF EXISTS {} WITH (FORCE)", db_name);
            sqlx::query(&drop_query).execute(template_pool).await?;
            Ok(())
        }

        pub fn get_database_name() -> Result<String, Box<dyn std::error::Error>> {
            dotenvy::dotenv().ok(); // Load .env file if it exists

            match std::env::var(db::keys::DBURL) {
                Ok(database_url) => {
                    let parsed_url = url::Url::parse(&database_url)?;
                    if parsed_url.scheme() == "postgres" || parsed_url.scheme() == "postgresql" {
                        match parsed_url
                            .path_segments()
                            .and_then(|segments| segments.last().map(|s| s.to_string()))
                        {
                            Some(sss) => Ok(sss),
                            None => Err("Error parsing".into()),
                        }
                    } else {
                        // Handle other database types if needed
                        Err("Error parsing".into())
                    }
                }
                Err(_) => {
                    // DATABASE_URL environment variable not found
                    Err("Error parsing".into())
                }
            }
        }
    }

    #[tokio::test]
    async fn test_song_queue() {
        let tm_pool = db_mgr::get_pool().await.unwrap();
        let db_name = db_mgr::generate_db_name().await;

        match db_mgr::create_database(&tm_pool, &db_name).await {
            Ok(_) => {
                println!("Success");
            }
            Err(err) => {
                assert!(false, "Error: {:?}", err);
            }
        }

        let pool = db_mgr::connect_to_db(&db_name).await.unwrap();
        db::migrations(&pool).await;

        let app = crate::init::routes()
            .await
            .layer(axum::Extension(pool))
            .layer(axum::extract::DefaultBodyLimit::max(1024 * 1024 * 1024))
            .layer(TimeoutLayer::new(Duration::from_secs(300)));

        // Create multipart form
        let mut form = MultipartForm::default();
        let _ = form.add_file("flac", "tests/Machine_gun/track01.flac");

        // Create request
        let content_type = form.content_type();
        let body = MultipartBody::from(form);
        let req = axum::http::Request::builder()
            .method(axum::http::Method::POST)
            .uri(crate::callers::endpoints::QUEUESONG)
            .header(axum::http::header::CONTENT_TYPE, content_type)
            .body(axum::body::Body::from_stream(body))
            .unwrap();

        // Send request
        match app.oneshot(req).await {
            Ok(response) => {
                let body = axum::body::to_bytes(response.into_body(), usize::MAX)
                    .await
                    .unwrap();
                println!("Body: {:?}", body);
                let resp: crate::callers::song::response::Response =
                    serde_json::from_slice(&body).unwrap();
                assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                assert_eq!(false, resp.data[0].is_nil(), "Should not be empty");
            }
            Err(err) => {
                assert!(false, "Error: {:?}", err);
            }
        };

        let _ = db_mgr::drop_database(&tm_pool, &db_name).await;
    }

    #[tokio::test]
    async fn test_song_metadata_queue() {
        let tm_pool = db_mgr::get_pool().await.unwrap();
        let db_name = db_mgr::generate_db_name().await;

        match db_mgr::create_database(&tm_pool, &db_name).await {
            Ok(_) => {
                println!("Success");
            }
            Err(err) => {
                assert!(false, "Error: {:?}", err);
            }
        }

        let pool = db_mgr::connect_to_db(&db_name).await.unwrap();
        db::migrations(&pool).await;

        let app = crate::init::routes()
            .await
            .layer(axum::Extension(pool))
            .layer(axum::extract::DefaultBodyLimit::max(1024 * 1024 * 1024))
            .layer(TimeoutLayer::new(Duration::from_secs(300)));

        // Create multipart form
        let mut form = MultipartForm::default();
        let _ = form.add_file("flac", "tests/Machine_gun/track01.flac");

        // Create request
        let content_type = form.content_type();
        let body = MultipartBody::from(form);
        let req = axum::http::Request::builder()
            .method(axum::http::Method::POST)
            .uri(crate::callers::endpoints::QUEUESONG)
            .header(axum::http::header::CONTENT_TYPE, content_type)
            .body(axum::body::Body::from_stream(body))
            .unwrap();

        // Send request
        match app.clone().oneshot(req).await {
            Ok(response) => {
                let body = axum::body::to_bytes(response.into_body(), usize::MAX)
                    .await
                    .unwrap();
                println!("Body: {:?}", body);
                let resp: crate::callers::song::response::Response =
                    serde_json::from_slice(&body).unwrap();
                assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                assert_eq!(false, resp.data[0].is_nil(), "Should not be empty");
                let new_payload: serde_json::Value = serde_json::json!(
                {
                        "id": resp.data[0],
                        "album" : "Machine Gun: The FillMore East First Show",
                        "album_artist" : "Jimi Hendrix",
                        "artist" : "Jimi Hendrix",
                        "disc" : 1,
                        "disc_count" : 1,
                        "duration" : 330,
                        "genre" : "Psychadelic Rock",
                        "title" : "Power of Soul",
                        "track" : 1,
                        "track_count" : 11,
                        "year" : 2016
                });

                match app
                    .clone()
                    .oneshot(
                        axum::http::Request::builder()
                            .method(axum::http::Method::POST)
                            .uri(crate::callers::endpoints::QUEUEMETADATA)
                            .header(axum::http::header::CONTENT_TYPE, "application/json")
                            .body(axum::body::Body::from(new_payload.to_string()))
                            .unwrap(),
                    )
                    .await
                {
                    Ok(response) => {
                        let body = axum::body::to_bytes(response.into_body(), usize::MAX)
                            .await
                            .unwrap();
                        let resp: crate::callers::song::response::Response =
                            serde_json::from_slice(&body).unwrap();
                        assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                    }
                    Err(err) => {
                        assert!(false, "Error: {:?}", err);
                    }
                }
            }
            Err(err) => {
                assert!(false, "Error: {:?}", err);
            }
        };

        let _ = db_mgr::drop_database(&tm_pool, &db_name).await;
    }
}
