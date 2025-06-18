pub mod callers;

pub mod db {

    use sqlx::postgres::PgPoolOptions;

    pub mod connection_settings {
        pub const MAXCONN: u32 = 10;
    }

    pub async fn create_pool() -> Result<sqlx::PgPool, sqlx::Error> {
        let database_url = icarus_envy::environment::get_db_url().await;
        println!("Database url: {:?}", database_url);

        PgPoolOptions::new()
            .max_connections(connection_settings::MAXCONN)
            .connect(&database_url)
            .await
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
    use axum::routing::{get, patch, post};
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
                crate::callers::endpoints::QUEUESONG,
                patch(crate::callers::song::endpoint::update_song_queue_status),
            )
            .route(
                crate::callers::endpoints::QUEUESONGDATA,
                get(crate::callers::song::endpoint::download_flac),
            )
            .route(
                crate::callers::endpoints::NEXTQUEUESONG,
                get(crate::callers::song::endpoint::fetch_queue_song),
            )
            .route(
                crate::callers::endpoints::QUEUESONGUPDATE,
                patch(crate::callers::song::endpoint::update_song_queue),
            )
            .route(
                crate::callers::endpoints::QUEUESONGDATAWIPE,
                patch(crate::callers::song::endpoint::wipe_data_from_song_queue),
            )
            .route(
                crate::callers::endpoints::QUEUEMETADATA,
                post(crate::callers::metadata::endpoint::queue_metadata),
            )
            .route(
                crate::callers::endpoints::QUEUEMETADATA,
                get(crate::callers::metadata::endpoint::fetch_metadata),
            )
            .route(
                crate::callers::endpoints::QUEUECOVERART,
                post(crate::callers::coverart::endpoint::queue),
            )
            .route(
                crate::callers::endpoints::QUEUECOVERARTDATA,
                get(crate::callers::coverart::endpoint::fetch_coverart_with_data),
            )
            .route(
                crate::callers::endpoints::QUEUECOVERART,
                get(crate::callers::coverart::endpoint::fetch_coverart_no_data),
            )
            .route(
                crate::callers::endpoints::QUEUECOVERARTLINK,
                patch(crate::callers::coverart::endpoint::link),
            )
            .route(
                crate::callers::endpoints::QUEUECOVERARTDATAWIPE,
                patch(crate::callers::coverart::endpoint::wipe_data_from_coverart_queue),
            )
            .route(
                crate::callers::endpoints::CREATESONG,
                post(crate::callers::song::endpoint::create_metadata),
            )
            .route(
                crate::callers::endpoints::CREATECOVERART,
                post(crate::callers::coverart::endpoint::create_coverart),
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

// TODO: Move elsewhere
fn get_full() -> String {
    get_address() + ":" + &get_port()
}
// TODO: Move elsewhere
fn get_address() -> String {
    String::from("0.0.0.0")
}

// TODO: Move elsewhere
fn get_port() -> String {
    String::from("3000")
}

// TODO: Move elsewhere
pub const ROOT: &str = "/";
// TODO: Move elsewhere
// basic handler that responds with a static string
pub async fn root() -> &'static str {
    "Hello, World!"
}

#[cfg(test)]
mod tests {
    use crate::db;

    use std::io::Write;
    use std::usize;

    use common_multipart_rfc7578::client::multipart::{
        Body as MultipartBody, Form as MultipartForm,
    };
    use tower::ServiceExt;

    mod db_mgr {
        use std::str::FromStr;

        pub const LIMIT: usize = 6;

        pub async fn get_pool() -> Result<sqlx::PgPool, sqlx::Error> {
            let tm_db_url = icarus_envy::environment::get_db_url().await;
            let tm_options = sqlx::postgres::PgConnectOptions::from_str(&tm_db_url).unwrap();
            sqlx::PgPool::connect_with(tm_options).await
        }

        pub async fn generate_db_name() -> String {
            let db_name = get_database_name().await.unwrap()
                + &"_"
                + &uuid::Uuid::new_v4().to_string()[..LIMIT];
            db_name
        }

        pub async fn connect_to_db(db_name: &str) -> Result<sqlx::PgPool, sqlx::Error> {
            let db_url = icarus_envy::environment::get_db_url().await;
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

        pub async fn get_database_name() -> Result<String, Box<dyn std::error::Error>> {
            let database_url = icarus_envy::environment::get_db_url().await;
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
    }

    mod init {
        pub async fn app(pool: sqlx::PgPool) -> axum::Router {
            crate::init::routes()
                .await
                .layer(axum::Extension(pool))
                .layer(axum::extract::DefaultBodyLimit::max(1024 * 1024 * 1024))
                .layer(tower_http::timeout::TimeoutLayer::new(
                    std::time::Duration::from_secs(300),
                ))
        }
    }

    async fn song_queue_req(
        app: &axum::Router,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
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
        app.clone().oneshot(req).await
    }

    async fn fetch_queue_req(
        app: &axum::Router,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let fetch_req = axum::http::Request::builder()
            .method(axum::http::Method::GET)
            .uri(crate::callers::endpoints::NEXTQUEUESONG)
            .header(axum::http::header::CONTENT_TYPE, "application/json")
            .body(axum::body::Body::empty())
            .unwrap();
        app.clone().oneshot(fetch_req).await
    }

    async fn fetch_metadata_queue_req(
        app: &axum::Router,
        id: &uuid::Uuid,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let uri = format!("{}?id={}", crate::callers::endpoints::QUEUEMETADATA, id);

        let req = axum::http::Request::builder()
            .method(axum::http::Method::GET)
            .uri(uri)
            .header(axum::http::header::CONTENT_TYPE, "application/json")
            .body(axum::body::Body::empty())
            .unwrap();

        app.clone().oneshot(req).await
    }

    async fn fetch_queue_data_req(
        app: &axum::Router,
        id: &uuid::Uuid,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let raw_uri = String::from(crate::callers::endpoints::QUEUESONGDATA);
        let end_index = raw_uri.len() - 4;
        let mut uri: String = (&raw_uri[..end_index]).to_string();
        uri += &id.to_string();
        let req = axum::http::Request::builder()
            .method(axum::http::Method::GET)
            .uri(uri)
            .header(axum::http::header::CONTENT_TYPE, "audio/flac")
            .body(axum::body::Body::empty())
            .unwrap();

        app.clone().oneshot(req).await
    }

    async fn upload_coverart_queue_req(
        app: &axum::Router,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let mut form = MultipartForm::default();
        let _ = form.add_file("jpg", "tests/Machine_gun/160809_machinegun.jpg");

        // Create request
        let content_type = form.content_type();
        let body = MultipartBody::from(form);

        let req = axum::http::Request::builder()
            .method(axum::http::Method::POST)
            .uri(crate::callers::endpoints::QUEUECOVERART)
            .header(axum::http::header::CONTENT_TYPE, content_type)
            .body(axum::body::Body::from_stream(body))
            .unwrap();

        // Send request
        app.clone().oneshot(req).await
    }

    async fn queue_metadata_req(
        app: &axum::Router,
        id: &uuid::Uuid,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let payload = payload_data::queue_metadata_payload_data(&id).await;

        let req = axum::http::Request::builder()
            .method(axum::http::Method::POST)
            .uri(crate::callers::endpoints::QUEUEMETADATA)
            .header(axum::http::header::CONTENT_TYPE, "application/json")
            .body(axum::body::Body::from(payload.to_string()))
            .unwrap();

        app.clone().oneshot(req).await
    }

    async fn coverart_queue_song_queue_link_req(
        app: &axum::Router,
        coverart_id: &uuid::Uuid,
        song_queue_id: &uuid::Uuid,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let payload = serde_json::json!(
        {
                "song_queue_id": song_queue_id,
                "coverart_id" : coverart_id,
        });
        let req = axum::http::Request::builder()
            .method(axum::http::Method::PATCH)
            .uri(crate::callers::endpoints::QUEUECOVERARTLINK)
            .header(axum::http::header::CONTENT_TYPE, "application/json")
            .body(axum::body::Body::from(payload.to_string()))
            .unwrap();

        app.clone().oneshot(req).await
    }

    async fn create_coverart_req(
        app: &axum::Router,
        song_id: &uuid::Uuid,
        coverart_id: &uuid::Uuid,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let payload = serde_json::json!({
            "song_id": song_id,
            "coverart_queue_id": coverart_id
        });
        let req = axum::http::Request::builder()
            .method(axum::http::Method::POST)
            .uri(crate::callers::endpoints::CREATECOVERART)
            .header(axum::http::header::CONTENT_TYPE, "application/json")
            .body(axum::body::Body::from(payload.to_string()))
            .unwrap();
        app.clone().oneshot(req).await
    }

    async fn create_song_req(
        app: &axum::Router,
        song_queue_id: &uuid::Uuid,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let payload = serde_json::json!({
            "title": "Power of Soul",
            "artist": "Jimmi Hendrix",
            "album_artist": "Jimmi Hendrix",
            "album": "Machine Gun",
            "genre": "Psychadelic Rock",
            "date": "2016-01-01",
            "track": 1,
            "disc": 1,
            "track_count": 11,
            "disc_count": 1,
            "duration": 330,
            "audio_type": "flac",
            "user_id": "d6e159c1-9648-4c85-81e5-52f502ff53e4",
            "song_queue_id": song_queue_id
        });

        let req = axum::http::Request::builder()
            .method(axum::http::Method::POST)
            .uri(crate::callers::endpoints::CREATESONG)
            .header(axum::http::header::CONTENT_TYPE, "application/json")
            .body(axum::body::Body::from(payload.to_string()))
            .unwrap();

        app.clone().oneshot(req).await
    }

    async fn update_song_queue_status_req(
        app: &axum::Router,
        song_queue_id: &uuid::Uuid,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let payload = serde_json::json!({
            "id": &song_queue_id,
            "status": crate::callers::song::status::READY
        });

        let req = axum::http::Request::builder()
            .method(axum::http::Method::PATCH)
            .uri(crate::callers::endpoints::QUEUESONG)
            .header(axum::http::header::CONTENT_TYPE, "application/json")
            .body(axum::body::Body::from(payload.to_string()))
            .unwrap();

        app.clone().oneshot(req).await
    }

    async fn get_queued_coverart(
        app: &axum::Router,
        coverart_queue_id: &uuid::Uuid,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let uri = format!(
            "{}?id={}",
            crate::callers::endpoints::QUEUECOVERART,
            coverart_queue_id
        );

        let req = axum::http::Request::builder()
            .method(axum::http::Method::GET)
            .uri(uri)
            .header(axum::http::header::CONTENT_TYPE, "application/json")
            .body(axum::body::Body::empty())
            .unwrap();

        app.clone().oneshot(req).await
    }

    mod sequence_flow {
        // Flow for queueing song
        pub async fn queue_song_flow(
            app: &axum::Router,
        ) -> Result<axum::response::Response, std::convert::Infallible> {
            match super::song_queue_req(&app).await {
                Ok(response) => {
                    let resp =
                        super::get_resp_data::<crate::callers::song::response::Response>(response)
                            .await;
                    assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                    assert_eq!(false, resp.data[0].is_nil(), "Should not be empty");
                    let song_queue_id = resp.data[0];
                    assert_eq!(false, song_queue_id.is_nil(), "Should not be empty");

                    match super::queue_metadata_req(&app, &resp.data[0]).await {
                        Ok(response) => {
                            let resp = super::get_resp_data::<
                                crate::callers::song::response::Response,
                            >(response)
                            .await;
                            assert_eq!(false, resp.data.is_empty(), "Should not be empty");

                            let id = resp.data[0];

                            match super::fetch_metadata_queue_req(&app, &id).await {
                                Ok(response) => Ok(response),
                                Err(err) => Err(err),
                            }
                        }
                    }
                }
                Err(err) => Err(err),
            }
        }

        // Flow for queueing coverart
        pub async fn queue_coverart_flow(
            app: &axum::Router,
            song_queue_id: &uuid::Uuid,
        ) -> Result<axum::response::Response, std::convert::Infallible> {
            match super::upload_coverart_queue_req(&app).await {
                Ok(response) => {
                    let resp =
                        super::get_resp_data::<crate::callers::coverart::response::Response>(
                            response,
                        )
                        .await;
                    assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                    let coverart_id = resp.data[0];
                    assert_eq!(false, coverart_id.is_nil(), "Should not be empty");

                    match super::coverart_queue_song_queue_link_req(
                        &app,
                        &coverart_id,
                        &song_queue_id,
                    )
                    .await
                    {
                        Ok(response) => {
                            let resp = super::get_resp_data::<
                                crate::callers::coverart::response::link::Response,
                            >(response)
                            .await;
                            assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                            let resp_coverart_id = resp.data[0].coverart_id;
                            let resp_song_queue_id = resp.data[0].song_queue_id;

                            assert_eq!(false, resp_coverart_id.is_nil(), "Should not be empty");
                            assert_eq!(false, resp_song_queue_id.is_nil(), "Should not be empty");

                            match super::get_queued_coverart(&app, &resp_coverart_id).await {
                                Ok(response) => Ok(response),
                                Err(err) => Err(err),
                            }
                        }
                    }
                }
            }
        }

        // Returns coverart response and song_queue_id
        pub async fn queue_song_and_coverart_flow(
            app: &axum::Router,
        ) -> Result<(axum::response::Response, uuid::Uuid), std::convert::Infallible> {
            match queue_song_flow(&app).await {
                Ok(song_response) => {
                    let resp = super::get_resp_data::<
                        crate::callers::metadata::response::fetch_metadata::Response,
                    >(song_response)
                    .await;
                    assert_eq!(false, resp.data.is_empty(), "Data should not be empty");
                    let song_queue_id = resp.data[0].song_queue_id;

                    match super::create_song_req(&app, &song_queue_id).await {
                        Ok(response) => {
                            let resp = super::get_resp_data::<
                                crate::callers::song::response::create_metadata::Response,
                            >(response)
                            .await;
                            assert_eq!(
                                false,
                                resp.data.is_empty(),
                                "No songs found, Response {:?}",
                                resp
                            );
                            let song = &resp.data[0];
                            let song_id = song.id;
                            assert_eq!(
                                false,
                                song_id.is_nil(),
                                "Song id should not be nil {:?}",
                                song
                            );

                            eprintln!("Song: {:?}", song);

                            match queue_coverart_flow(&app, &song_queue_id).await {
                                Ok(response) => Ok((response, song_queue_id)),
                                Err(err) => {
                                    assert!(false, "Error: {:?}", err);
                                    Err(err)
                                }
                            }
                        }
                        Err(err) => {
                            assert!(false, "Error: {:?}", err);
                            Err(err)
                        }
                    }
                }
                Err(err) => {
                    assert!(false, "Error: {:?}", err);
                    Err(err)
                }
            }
        }
    }

    pub async fn resp_to_bytes(
        response: axum::response::Response,
    ) -> Result<axum::body::Bytes, axum::Error> {
        axum::body::to_bytes(response.into_body(), usize::MAX).await
    }

    pub async fn get_resp_data<Data>(response: axum::response::Response) -> Data
    where
        Data: for<'a> serde::Deserialize<'a>,
    {
        let body = resp_to_bytes(response).await.unwrap();
        serde_json::from_slice(&body).unwrap()
    }

    pub mod payload_data {
        pub async fn queue_metadata_payload_data(id: &uuid::Uuid) -> serde_json::Value {
            serde_json::json!(
            {
                    "song_queue_id": id,
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
            })
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

        let app = init::app(pool).await;

        // Send request
        match song_queue_req(&app).await {
            Ok(response) => {
                let resp =
                    get_resp_data::<crate::callers::song::response::Response>(response).await;
                assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                assert_eq!(false, resp.data[0].is_nil(), "Should not be empty");
            }
            Err(err) => {
                assert!(false, "Error: {:?}", err);
            }
        };

        let _ = db_mgr::drop_database(&tm_pool, &db_name).await;
    }

    mod special_area {

        use super::*;

        use std::io::Write;

        #[tokio::test]
        async fn test_song_fetch_queue_item() {
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

            let app = init::app(pool).await;

            match sequence_flow::queue_song_and_coverart_flow(&app).await {
                Ok((resp_one, song_queue_id)) => {
                    let resp = get_resp_data::<
                        crate::callers::coverart::response::fetch_coverart_no_data::Response,
                    >(resp_one)
                    .await;
                    assert_eq!(false, resp.data.is_empty(), "Should not be empty");

                    let _resp_coverart_queue_id = resp.data[0].id;

                    let old = crate::callers::song::status::PENDING;
                    let target_status = crate::callers::song::status::READY;

                    match update_song_queue_status_req(&app, &song_queue_id).await {
                        Ok(response) => {
                            let resp = get_resp_data::<
                                crate::callers::song::response::update_status::Response,
                            >(response)
                            .await;
                            assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                            let changed_status = &resp.data[0];

                            assert_eq!(
                                *old, changed_status.old_status,
                                "Old status does not match"
                            );
                            assert_eq!(
                                target_status, changed_status.new_status,
                                "New status does not match"
                            );

                            match fetch_queue_req(&app).await {
                                Ok(response) => {
                                    let resp = get_resp_data::<
                                        crate::callers::song::response::fetch_queue_song::Response,
                                    >(response)
                                    .await;
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
                    }
                }
                Err(err) => {
                    assert!(false, "Error: {:?}", err);
                }
            };

            let _ = db_mgr::drop_database(&tm_pool, &db_name).await;
        }

        #[tokio::test]
        async fn test_update_song_from_queue() {
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

            let app = init::app(pool).await;

            // Send request
            match song_queue_req(&app).await {
                Ok(response) => {
                    let resp =
                        get_resp_data::<crate::callers::song::response::Response>(response).await;
                    assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                    assert_eq!(false, resp.data[0].is_nil(), "Should not be empty");

                    let id = &resp.data[0];

                    match fetch_queue_data_req(&app, &id).await {
                        Ok(response) => match resp_to_bytes(response).await {
                            Ok(bytes) => {
                                assert_eq!(
                                    false,
                                    bytes.is_empty(),
                                    "Queued data should not be empty"
                                );

                                let temp_file =
                                    tempfile::tempdir().expect("Could not create test directory");
                                let test_dir = String::from(temp_file.path().to_str().unwrap());
                                let new_file = format!("{}/new_file.flac", test_dir);

                                let mut file = std::fs::File::create(&new_file).unwrap();
                                file.write_all(&bytes).unwrap();

                                let mut form = MultipartForm::default();
                                let _ = form.add_file("flac", new_file);

                                // Create request
                                let content_type = form.content_type();
                                let body = MultipartBody::from(form);

                                let raw_uri =
                                    String::from(crate::callers::endpoints::QUEUESONGUPDATE);
                                let end_index = raw_uri.len() - 5;

                                let uri = format!(
                                    "{}/{}",
                                    (&raw_uri[..end_index]).to_string(),
                                    id.to_string()
                                );

                                match app
                                    .clone()
                                    .oneshot(
                                        axum::http::Request::builder()
                                            .method(axum::http::Method::PATCH)
                                            .uri(uri)
                                            .header(axum::http::header::CONTENT_TYPE, content_type)
                                            .body(axum::body::Body::from_stream(body))
                                            .unwrap(),
                                    )
                                    .await
                                {
                                    Ok(response) => {
                                        let resp = get_resp_data::<
                                            crate::callers::song::response::update_song_queue::Response,
                                        >(response)
                                        .await;
                                        assert_eq!(
                                            false,
                                            resp.data.is_empty(),
                                            "Should not be empty"
                                        );
                                    }
                                    Err(err) => {
                                        assert!(false, "Error: {:?}", err);
                                    }
                                }
                            }
                            Err(err) => {
                                assert!(false, "Error: {:?}", err);
                            }
                        },
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

        #[tokio::test]
        async fn test_song_fetch_queue_data() {
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

            let app = init::app(pool).await;

            // Send request
            match song_queue_req(&app).await {
                Ok(response) => {
                    let resp =
                        get_resp_data::<crate::callers::song::response::Response>(response).await;
                    assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                    assert_eq!(false, resp.data[0].is_nil(), "Should not be empty");
                    let id = resp.data[0];

                    match fetch_queue_data_req(&app, &id).await {
                        Ok(response) => match resp_to_bytes(response).await {
                            Ok(bytes) => {
                                assert_eq!(
                                    false,
                                    bytes.is_empty(),
                                    "Queued data should not be empty"
                                );
                            }
                            Err(err) => {
                                assert!(false, "Error: {:?}", err);
                            }
                        },
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

        #[tokio::test]
        async fn test_song_queue_update_status() {
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

            let app = init::app(pool).await;

            match sequence_flow::queue_song_and_coverart_flow(&app).await {
                Ok((resp_one, song_queue_id)) => {
                    let resp = get_resp_data::<
                        crate::callers::coverart::response::fetch_coverart_no_data::Response,
                    >(resp_one)
                    .await;
                    assert_eq!(false, resp.data.is_empty(), "Should not be empty");

                    let _resp_coverart_queue_id = resp.data[0].id;

                    let old = crate::callers::song::status::PENDING;
                    let done = crate::callers::song::status::READY;

                    match update_song_queue_status_req(&app, &song_queue_id).await {
                        Ok(response) => {
                            let resp = get_resp_data::<
                                crate::callers::song::response::update_status::Response,
                            >(response)
                            .await;
                            assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                            let changed_status = &resp.data[0];

                            assert_eq!(
                                *old, changed_status.old_status,
                                "Old status does not match"
                            );
                            assert_eq!(
                                done, changed_status.new_status,
                                "New status does not match"
                            );
                        }
                        Err(err) => {
                            assert!(false, "Error: {:?}", err);
                        }
                    }
                }
                Err(err) => {
                    assert!(false, "Error: {:?}", err);
                }
            }

            let _ = db_mgr::drop_database(&tm_pool, &db_name).await;
        }
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

        let app = init::app(pool).await;

        // Send request
        match song_queue_req(&app).await {
            Ok(response) => {
                let resp =
                    get_resp_data::<crate::callers::song::response::Response>(response).await;
                assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                assert_eq!(false, resp.data[0].is_nil(), "Should not be empty");

                match queue_metadata_req(&app, &resp.data[0]).await {
                    Ok(response) => {
                        let resp =
                            get_resp_data::<crate::callers::song::response::Response>(response)
                                .await;
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

    #[tokio::test]
    async fn test_get_metadata_queue() {
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

        let app = init::app(pool).await;

        // Send request
        match sequence_flow::queue_song_flow(&app).await {
            Ok(response) => {
                let resp = get_resp_data::<
                    crate::callers::metadata::response::fetch_metadata::Response,
                >(response)
                .await;
                assert_eq!(false, resp.data.is_empty(), "Data should not be empty");
            }
            Err(err) => {
                assert!(false, "Error: {:?}", err);
            }
        };

        let _ = db_mgr::drop_database(&tm_pool, &db_name).await;
    }

    #[tokio::test]
    async fn test_song_coverart_queue() {
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

        let app = init::app(pool).await;

        // Send request
        match upload_coverart_queue_req(&app).await {
            Ok(response) => {
                let resp =
                    get_resp_data::<crate::callers::coverart::response::Response>(response).await;
                assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                let id = resp.data[0];
                assert_eq!(false, id.is_nil(), "Should not be empty");
            }
            Err(err) => {
                assert!(false, "Error: {:?}", err);
            }
        };

        let _ = db_mgr::drop_database(&tm_pool, &db_name).await;
    }

    #[tokio::test]
    async fn test_song_coverart_queue_link() {
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

        let app = init::app(pool).await;

        match song_queue_req(&app).await {
            Ok(response) => {
                let resp =
                    get_resp_data::<crate::callers::coverart::response::Response>(response).await;
                assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                let song_queue_id = resp.data[0];
                assert_eq!(false, song_queue_id.is_nil(), "Should not be empty");

                // Send request
                match upload_coverart_queue_req(&app).await {
                    Ok(response) => {
                        let resp =
                            get_resp_data::<crate::callers::coverart::response::Response>(response)
                                .await;
                        assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                        let coverart_id = resp.data[0];
                        assert_eq!(false, coverart_id.is_nil(), "Should not be empty");

                        match coverart_queue_song_queue_link_req(&app, &coverart_id, &song_queue_id)
                            .await
                        {
                            Ok(response) => {
                                let resp = get_resp_data::<
                                    crate::callers::coverart::response::link::Response,
                                >(response)
                                .await;
                                assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                                let resp_coverart_id = resp.data[0].coverart_id;
                                let resp_song_queue_id = resp.data[0].song_queue_id;

                                assert_eq!(false, resp_coverart_id.is_nil(), "Should not be empty");
                                assert_eq!(
                                    false,
                                    resp_song_queue_id.is_nil(),
                                    "Should not be empty"
                                );
                            }
                            Err(err) => {
                                assert!(false, "Error: {:?}", err);
                            }
                        }
                    }
                    Err(err) => {
                        assert!(false, "Error: {:?}", err);
                    }
                }
            }
            Err(err) => {
                assert!(false, "Error: {:?}", err);
            }
        }

        let _ = db_mgr::drop_database(&tm_pool, &db_name).await;
    }

    #[tokio::test]
    async fn test_fetch_coverart_queue_without_data() {
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

        let app = init::app(pool).await;

        match song_queue_req(&app).await {
            Ok(response) => {
                let resp =
                    get_resp_data::<crate::callers::coverart::response::Response>(response).await;
                assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                let song_queue_id = resp.data[0];
                assert_eq!(false, song_queue_id.is_nil(), "Should not be empty");

                match sequence_flow::queue_coverart_flow(&app, &song_queue_id).await {
                    Ok(response) => {
                        let resp = get_resp_data::<
                            crate::callers::coverart::response::fetch_coverart_no_data::Response,
                        >(response)
                        .await;
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
        }

        let _ = db_mgr::drop_database(&tm_pool, &db_name).await;
    }

    #[tokio::test]
    async fn test_fetch_coverart_queue_with_data() {
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

        let app = init::app(pool).await;

        match song_queue_req(&app).await {
            Ok(response) => {
                let resp =
                    get_resp_data::<crate::callers::coverart::response::Response>(response).await;
                assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                let song_queue_id = resp.data[0];
                assert_eq!(false, song_queue_id.is_nil(), "Should not be empty");

                // Send request
                match upload_coverart_queue_req(&app).await {
                    Ok(response) => {
                        let resp =
                            get_resp_data::<crate::callers::coverart::response::Response>(response)
                                .await;
                        assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                        let coverart_id = resp.data[0];
                        assert_eq!(false, coverart_id.is_nil(), "Should not be empty");

                        match coverart_queue_song_queue_link_req(&app, &coverart_id, &song_queue_id)
                            .await
                        {
                            Ok(response) => {
                                let resp = get_resp_data::<
                                    crate::callers::coverart::response::link::Response,
                                >(response)
                                .await;
                                assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                                let resp_coverart_id = resp.data[0].coverart_id;
                                let resp_song_queue_id = resp.data[0].song_queue_id;

                                assert_eq!(false, resp_coverart_id.is_nil(), "Should not be empty");
                                assert_eq!(
                                    false,
                                    resp_song_queue_id.is_nil(),
                                    "Should not be empty"
                                );

                                let raw_uri =
                                    String::from(crate::callers::endpoints::QUEUECOVERARTDATA);
                                let end_index = raw_uri.len() - 5;
                                let uri = format!(
                                    "{}/{}",
                                    (&raw_uri[..end_index]).to_string(),
                                    resp_coverart_id
                                );
                                println!("Uri: {:?}", uri);

                                match app
                                    .clone()
                                    .oneshot(
                                        axum::http::Request::builder()
                                            .method(axum::http::Method::GET)
                                            .uri(uri)
                                            .header(axum::http::header::CONTENT_TYPE, "image/jpeg")
                                            .body(axum::body::Body::empty())
                                            .unwrap(),
                                    )
                                    .await
                                {
                                    Ok(response) => match resp_to_bytes(response).await {
                                        Ok(bytes) => {
                                            assert_eq!(
                                                false,
                                                bytes.is_empty(),
                                                "Downloaded coverart data should not be empty"
                                            );
                                            let temp_file = tempfile::tempdir()
                                                .expect("Could not create test directory");
                                            let test_dir =
                                                String::from(temp_file.path().to_str().unwrap());
                                            let new_file = format!("{}/new_image.jpeg", test_dir);

                                            let mut file =
                                                std::fs::File::create(&new_file).unwrap();
                                            file.write_all(&bytes).unwrap();
                                        }
                                        Err(err) => {
                                            assert!(false, "Error: {:?}", err);
                                        }
                                    },
                                    Err(err) => {
                                        assert!(false, "Error: {:?}", err);
                                    }
                                }
                            }
                            Err(err) => {
                                assert!(false, "Error: {:?}", err);
                            }
                        }
                    }
                    Err(err) => {
                        assert!(false, "Error: {:?}", err);
                    }
                }
            }
            Err(err) => {
                assert!(false, "Error: {:?}", err);
            }
        }

        let _ = db_mgr::drop_database(&tm_pool, &db_name).await;
    }

    #[tokio::test]
    async fn test_create_song() {
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

        let app = init::app(pool).await;

        // Send request
        match sequence_flow::queue_song_flow(&app).await {
            Ok(response) => {
                let resp = get_resp_data::<
                    crate::callers::metadata::response::fetch_metadata::Response,
                >(response)
                .await;
                assert_eq!(false, resp.data.is_empty(), "Data should not be empty");
                let song_q_id = resp.data[0].song_queue_id;

                match create_song_req(&app, &song_q_id).await {
                    Ok(response) => {
                        let resp = get_resp_data::<
                            crate::callers::song::response::create_metadata::Response,
                        >(response)
                        .await;
                        assert_eq!(
                            false,
                            resp.data.is_empty(),
                            "No songs found, Response {:?}",
                            resp
                        );
                        let song = &resp.data[0];
                        let song_id = song.id;
                        assert_eq!(
                            false,
                            song_id.is_nil(),
                            "Song id should not be nil {:?}",
                            song
                        );
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

    #[tokio::test]
    async fn test_create_coverart() {
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

        let app = init::app(pool).await;

        // Send request
        match sequence_flow::queue_song_flow(&app).await {
            Ok(response) => {
                let resp = get_resp_data::<
                    crate::callers::metadata::response::fetch_metadata::Response,
                >(response)
                .await;
                assert_eq!(false, resp.data.is_empty(), "Data should not be empty");
                let song_queue_id = resp.data[0].song_queue_id;

                match create_song_req(&app, &song_queue_id).await {
                    Ok(response) => {
                        let resp = get_resp_data::<
                            crate::callers::song::response::create_metadata::Response,
                        >(response)
                        .await;
                        assert_eq!(
                            false,
                            resp.data.is_empty(),
                            "No songs found, Response {:?}",
                            resp
                        );
                        let song = &resp.data[0];
                        let song_id = song.id;
                        assert_eq!(
                            false,
                            song_id.is_nil(),
                            "Song id should not be nil {:?}",
                            song
                        );

                        eprintln!("Song: {:?}", song);

                        match sequence_flow::queue_coverart_flow(&app, &song_queue_id).await {
                            Ok(response) => {
                                let resp = get_resp_data::<
                                                    crate::callers::coverart::response::fetch_coverart_no_data::Response,
                                                >(response)
                                                .await;
                                assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                                let resp_queue_coverart_id = resp.data[0].id;

                                match create_coverart_req(&app, &song_id, &resp_queue_coverart_id)
                                    .await
                                {
                                    Ok(response) => {
                                        let resp = get_resp_data::<
                                                            crate::callers::coverart::response::create_coverart::Response,
                                                        >(response)
                                                        .await;
                                        assert_eq!(
                                            false,
                                            resp.data.is_empty(),
                                            "Should not be empty"
                                        );
                                    }
                                    Err(err) => {
                                        assert!(false, "Error: {:?}", err);
                                    }
                                }
                            }
                            Err(err) => {
                                assert!(false, "Error: {:?}", err);
                            }
                        }
                    }
                }
            }
            Err(err) => {
                assert!(false, "Error: {:?}", err);
            }
        };

        let _ = db_mgr::drop_database(&tm_pool, &db_name).await;
    }

    #[tokio::test]
    async fn test_wipe_data_from_song_queue() {
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

        let app = init::app(pool).await;

        // Send request
        match sequence_flow::queue_song_flow(&app).await {
            Ok(response) => {
                let resp = get_resp_data::<
                    crate::callers::metadata::response::fetch_metadata::Response,
                >(response)
                .await;
                assert_eq!(false, resp.data.is_empty(), "Data should not be empty");
                let song_q_id = resp.data[0].song_queue_id;

                match create_song_req(&app, &song_q_id).await {
                    Ok(response) => {
                        let resp = get_resp_data::<
                            crate::callers::song::response::create_metadata::Response,
                        >(response)
                        .await;
                        assert_eq!(
                            false,
                            resp.data.is_empty(),
                            "No songs found, Response {:?}",
                            resp
                        );
                        let song = &resp.data[0];
                        let song_id = song.id;
                        assert_eq!(
                            false,
                            song_id.is_nil(),
                            "Song id should not be nil {:?}",
                            song
                        );

                        let payload = serde_json::json!({
                            "song_queue_id": song_q_id
                        });

                        match app
                            .clone()
                            .oneshot(
                                axum::http::Request::builder()
                                    .method(axum::http::Method::PATCH)
                                    .uri(crate::callers::endpoints::QUEUESONGDATAWIPE)
                                    .header(axum::http::header::CONTENT_TYPE, "application/json")
                                    .body(axum::body::Body::from(payload.to_string()))
                                    .unwrap(),
                            )
                            .await
                        {
                            Ok(response) => {
                                let resp = get_resp_data::<crate::callers::song::response::wipe_data_from_song_queue::Response>(response).await;
                                assert_eq!(
                                    false,
                                    resp.data.is_empty(),
                                    "Failure in wiping data from song queue {:?}",
                                    resp
                                );

                                let returned_id = &resp.data[0];
                                assert_eq!(
                                    false,
                                    returned_id.is_nil(),
                                    "Returned id should not be nil {:?}",
                                    returned_id
                                );
                                assert_eq!(
                                    *returned_id, song_q_id,
                                    "Returned id does not match sent id {:?} {:?}",
                                    returned_id, song_q_id
                                );
                            }
                            Err(err) => {
                                assert!(false, "Error: {:?}", err);
                            }
                        }
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

    #[tokio::test]
    async fn test_wipe_data_from_coverart_queue() {
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

        let app = init::app(pool).await;

        match sequence_flow::queue_song_flow(&app).await {
            Ok(response) => {
                let resp = get_resp_data::<
                    crate::callers::metadata::response::fetch_metadata::Response,
                >(response)
                .await;
                assert_eq!(false, resp.data.is_empty(), "Data should not be empty");
                let song_queue_id = resp.data[0].song_queue_id;

                match create_song_req(&app, &song_queue_id).await {
                    Ok(response) => {
                        let resp = get_resp_data::<
                            crate::callers::song::response::create_metadata::Response,
                        >(response)
                        .await;
                        assert_eq!(
                            false,
                            resp.data.is_empty(),
                            "No songs found, Response {:?}",
                            resp
                        );
                        let song = &resp.data[0];
                        let song_id = song.id;
                        assert_eq!(
                            false,
                            song_id.is_nil(),
                            "Song id should not be nil {:?}",
                            song
                        );

                        eprintln!("Song: {:?}", song);

                        match sequence_flow::queue_coverart_flow(&app, &song_queue_id).await {
                            Ok(response) => {
                                let resp = get_resp_data::<
                                    crate::callers::coverart::response::fetch_coverart_no_data::Response,
                                >(response)
                                .await;
                                assert_eq!(false, resp.data.is_empty(), "Should not be empty");

                                let resp_coverart_queue_id = resp.data[0].id;

                                let payload = serde_json::json!({
                                    "coverart_queue_id": resp_coverart_queue_id
                                });

                                match app
                                    .clone()
                                    .oneshot(
                                        axum::http::Request::builder()
                                            .method(axum::http::Method::PATCH)
                                            .uri(crate::callers::endpoints::QUEUECOVERARTDATAWIPE)
                                            .header(
                                                axum::http::header::CONTENT_TYPE,
                                                "application/json",
                                            )
                                            .body(axum::body::Body::from(payload.to_string()))
                                            .unwrap(),
                                    )
                                    .await
                                {
                                    Ok(response) => {
                                        let resp = get_resp_data::<
                                            crate::callers::coverart::response::wipe_data_from_coverart_queue::Response,
                                        >(response)
                                        .await;
                                        assert_eq!(
                                            false,
                                            resp.data.is_empty(),
                                            "Should not be empty"
                                        );
                                    }
                                    Err(err) => {
                                        assert!(false, "Error: {:?}", err);
                                    }
                                }
                            }
                            Err(err) => {
                                assert!(false, "Error: {:?}", err);
                            }
                        }
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
