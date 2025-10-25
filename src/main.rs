pub mod auth;
pub mod callers;
pub mod repo;

pub mod db {

    use sqlx::postgres::PgPoolOptions;

    pub mod connection_settings {
        pub const MAXCONN: u32 = 10;
    }

    pub async fn create_pool() -> Result<sqlx::PgPool, sqlx::Error> {
        let database_url = icarus_envy::environment::get_db_url().await.value;
        println!("Database url: {database_url}");

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
    use std::time::Duration;

    use axum::routing::{delete, get, patch, post};
    use tower_http::timeout::TimeoutLayer;
    use utoipa::OpenApi;

    use crate::callers::coverart as coverart_caller;
    use crate::callers::metadata as metadata_caller;
    use crate::callers::queue::coverart as coverart_queue_callers;
    use crate::callers::song as song_caller;
    use coverart_caller::endpoint as coverart_endpoints;
    use coverart_caller::response as coverart_responses;
    use coverart_queue_callers::endpoint as coverart_queue_endpoints;
    use coverart_queue_callers::response as coverart_queue_responses;
    use metadata_caller::endpoint as metadata_endpoints;
    use metadata_caller::response as metadata_responses;
    use song_caller::endpoint as song_endpoints;
    use song_caller::response as song_responses;

    mod cors {
        pub async fn configure_cors() -> tower_http::cors::CorsLayer {
            let cors = tower_http::cors::CorsLayer::new()
                .allow_methods([
                    axum::http::Method::GET,
                    axum::http::Method::POST,
                    axum::http::Method::PUT,
                    axum::http::Method::DELETE,
                ]) // Specify allowed methods:cite[2]
                .allow_headers([
                    axum::http::header::CONTENT_TYPE,
                    axum::http::header::AUTHORIZATION,
                ]) // Specify allowed headers:cite[2]
                .allow_credentials(true) // If you need to send cookies or authentication headers:cite[2]
                .max_age(std::time::Duration::from_secs(3600)); // Cache the preflight response for 1 hour:cite[2]

            // Dynamically set the allowed origin based on the environment
            match std::env::var(icarus_envy::keys::APP_ENV).as_deref() {
                Ok("production") => {
                    // In production, allow only your specific, trusted origins
                    let allowed_origins_env = icarus_envy::environment::get_allowed_origins().await;
                    match icarus_envy::utility::delimitize(&allowed_origins_env) {
                        Ok(alwd) => {
                            let allowed_origins: Vec<axum::http::HeaderValue> = alwd
                                .into_iter()
                                .map(|a| a.parse::<axum::http::HeaderValue>().unwrap())
                                .collect();
                            cors.allow_origin(allowed_origins)
                        }
                        Err(err) => {
                            eprintln!("Error getting allowed origins: Error: {err:?}");
                            std::process::exit(-1);
                        }
                    }
                }
                _ => {
                    // Development (default): Allow localhost origins
                    cors.allow_origin(vec![
                        "http://localhost:8000".parse().unwrap(),
                        "http://127.0.0.1:8000".parse().unwrap(),
                        "http://localhost:4200".parse().unwrap(),
                        "http://127.0.0.1:4200".parse().unwrap(),
                    ])
                }
            }
        }
    }

    #[derive(utoipa::OpenApi)]
    #[openapi(
        paths(song_endpoints::queue_song, song_endpoints::link_user_id, song_endpoints::fetch_queue_song, song_endpoints::download_flac,
            song_endpoints::update_song_queue_status, song_endpoints::update_song_queue, song_endpoints::create_metadata, song_endpoints::wipe_data_from_song_queue, song_endpoints::get_songs, song_endpoints::get_all_songs, song_endpoints::stream_song, song_endpoints::download_song,
            song_endpoints::delete_song, coverart_queue_endpoints::queue, coverart_queue_endpoints::link, coverart_queue_endpoints::fetch_coverart_no_data,
            coverart_queue_endpoints::fetch_coverart_with_data, coverart_endpoints::create_coverart, coverart_queue_endpoints::wipe_data_from_coverart_queue,
            coverart_endpoints::get_coverart, coverart_endpoints::download_coverart,
            metadata_endpoints::queue_metadata, metadata_endpoints::fetch_metadata),
        components(schemas(song_responses::Response, song_responses::link_user_id::Response, song_responses::fetch_queue_song::Response,
                song_responses::update_status::Response, song_responses::update_song_queue::Response, song_responses::create_metadata::Response,
                song_responses::wipe_data_from_song_queue::Response, song_responses::get_songs::Response, song_responses::delete_song::Response,
                coverart_queue_responses::queue::Response, coverart_queue_responses::link::Response, coverart_queue_responses::fetch_coverart_no_data::Response,
                coverart_queue_responses::fetch_coverart_with_data::Response, coverart_responses::create_coverart::Response,
                coverart_queue_responses::wipe_data_from_coverart_queue::Response, coverart_responses::get_coverart::Response,
                metadata_responses::queue_metadata::Response, metadata_responses::fetch_metadata::Response)),
        tags(
            (name = "Icarus API", description = "Web API to manage music")
        )
    )]
    struct ApiDoc;

    pub async fn routes() -> axum::Router {
        axum::Router::new()
            .route(crate::ROOT, get(crate::root))
            .route(
                crate::callers::endpoints::QUEUESONG,
                post(crate::callers::song::endpoint::queue_song).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::QUEUESONG,
                patch(crate::callers::song::endpoint::update_song_queue_status).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::QUEUESONGLINKUSERID,
                patch(crate::callers::song::endpoint::link_user_id).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::QUEUESONGDATA,
                get(crate::callers::song::endpoint::download_flac).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::NEXTQUEUESONG,
                get(crate::callers::song::endpoint::fetch_queue_song).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::QUEUESONGUPDATE,
                patch(crate::callers::song::endpoint::update_song_queue).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::QUEUESONGDATAWIPE,
                patch(crate::callers::song::endpoint::wipe_data_from_song_queue).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::QUEUEMETADATA,
                post(crate::callers::metadata::endpoint::queue_metadata).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::QUEUEMETADATA,
                get(crate::callers::metadata::endpoint::fetch_metadata).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::QUEUECOVERART,
                post(crate::callers::queue::coverart::endpoint::queue).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::QUEUECOVERARTDATA,
                get(crate::callers::queue::coverart::endpoint::fetch_coverart_with_data)
                    .route_layer(axum::middleware::from_fn(
                        crate::auth::auth::<axum::body::Body>,
                    )),
            )
            .route(
                crate::callers::endpoints::QUEUECOVERART,
                get(crate::callers::queue::coverart::endpoint::fetch_coverart_no_data).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::QUEUECOVERARTLINK,
                patch(crate::callers::queue::coverart::endpoint::link).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::QUEUECOVERARTDATAWIPE,
                patch(crate::callers::queue::coverart::endpoint::wipe_data_from_coverart_queue)
                    .route_layer(axum::middleware::from_fn(
                        crate::auth::auth::<axum::body::Body>,
                    )),
            )
            .route(
                crate::callers::endpoints::CREATESONG,
                post(crate::callers::song::endpoint::create_metadata).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::CREATECOVERART,
                post(crate::callers::coverart::endpoint::create_coverart).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::GETSONGS,
                get(crate::callers::song::endpoint::get_songs).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::GETCOVERART,
                get(crate::callers::coverart::endpoint::get_coverart).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::DOWNLOADCOVERART,
                get(crate::callers::coverart::endpoint::download_coverart).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::STREAMSONG,
                get(crate::callers::song::endpoint::stream_song).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::DOWNLOADSONG,
                get(crate::callers::song::endpoint::download_song).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::DELETESONG,
                delete(crate::callers::song::endpoint::delete_song).route_layer(
                    axum::middleware::from_fn(crate::auth::auth::<axum::body::Body>),
                ),
            )
            .route(
                crate::callers::endpoints::GETALLSONGS,
                get(crate::callers::song::endpoint::get_all_songs),
            )
            .layer(cors::configure_cors().await)
    }

    pub async fn app() -> axum::Router {
        let pool = crate::db::create_pool()
            .await
            .expect("Failed to create pool");
        // TODO: Look into handling this. Seems redundant to run migrations multiple times
        crate::db::migrations(&pool).await;

        let cors = cors::configure_cors().await;

        routes()
            .await
            .merge(
                utoipa_swagger_ui::SwaggerUi::new("/swagger-ui")
                    .url("/api-docs/openapi.json", ApiDoc::openapi()),
            )
            .layer(axum::Extension(pool))
            .layer(axum::extract::DefaultBodyLimit::max(1024 * 1024 * 1024))
            .layer(TimeoutLayer::new(Duration::from_secs(300)))
            .layer(cors)
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
    String::from("8000")
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
            let tm_db_url = icarus_envy::environment::get_db_url().await.value;
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
            let db_url = icarus_envy::environment::get_db_url().await.value;
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
            let database_url = icarus_envy::environment::get_db_url().await.value;
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

        pub async fn migrations(pool: &sqlx::PgPool) {
            // Run migrations using the sqlx::migrate! macro
            // Assumes your test migrations are in a ./test_migrations folder relative to Cargo.toml
            sqlx::migrate!("./test_migrations")
                .run(pool)
                .await
                .expect("Failed to run migrations");
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

    pub fn token_fields() -> (String, String, String) {
        (
            String::from("What a twist!"),
            String::from("icarus_test"),
            String::from("icarus"),
        )
    }

    pub const TEST_USER_ID: uuid::Uuid = uuid::uuid!("cc938368-615a-4694-b2ca-6e122fa31c52");

    pub async fn test_token() -> Result<String, josekit::JoseError> {
        let key: String = icarus_envy::environment::get_secret_main_key().await.value;
        let (message, issuer, audience) = token_fields();

        let token_resource = icarus_models::token::TokenResource {
            message: message,
            issuer: issuer,
            audiences: vec![audience],
            id: TEST_USER_ID,
        };

        match icarus_models::token::create_token(&key, &token_resource, time::Duration::hours(1)) {
            Ok((access_token, _some_time)) => Ok(access_token),
            Err(err) => Err(err),
        }
    }

    pub async fn bearer_auth() -> String {
        let token = match test_token().await {
            Ok(access_token) => access_token,
            Err(err) => {
                assert!(false, "Error: {err:?}");
                String::new()
            }
        };

        format!("Bearer {token}")
    }

    // TODO: Put the *_req() functions in their own module
    async fn song_queue_req(
        app: &axum::Router,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        // Create multipart form
        let mut form = MultipartForm::default();
        let _ = form.add_file("flac", "tests/I/track01.flac");

        // Create request
        let content_type = form.content_type();
        let body = MultipartBody::from(form);
        let req = axum::http::Request::builder()
            .method(axum::http::Method::POST)
            .uri(crate::callers::endpoints::QUEUESONG)
            .header(axum::http::header::CONTENT_TYPE, content_type)
            .header(axum::http::header::AUTHORIZATION, bearer_auth().await)
            .body(axum::body::Body::from_stream(body))
            .unwrap();
        app.clone().oneshot(req).await
    }

    async fn song_queue_link_req(
        app: &axum::Router,
        song_queue_id: &uuid::Uuid,
        user_id: &uuid::Uuid,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let payload = serde_json::json!({
            "song_queue_id": song_queue_id,
            "user_id": user_id
        });

        let req = axum::http::Request::builder()
            .method(axum::http::Method::PATCH)
            .uri(crate::callers::endpoints::QUEUESONGLINKUSERID)
            .header(axum::http::header::CONTENT_TYPE, "application/json")
            .header(axum::http::header::AUTHORIZATION, bearer_auth().await)
            .body(axum::body::Body::from(payload.to_string()))
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
            .header(axum::http::header::AUTHORIZATION, bearer_auth().await)
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
            .header(axum::http::header::AUTHORIZATION, bearer_auth().await)
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
            .header(axum::http::header::AUTHORIZATION, bearer_auth().await)
            .body(axum::body::Body::empty())
            .unwrap();

        app.clone().oneshot(req).await
    }

    async fn upload_coverart_queue_req(
        app: &axum::Router,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let mut form = MultipartForm::default();
        let _ = form.add_file("jpg", "tests/I/Coverart-1.jpg");

        // Create request
        let content_type = form.content_type();
        let body = MultipartBody::from(form);

        let req = axum::http::Request::builder()
            .method(axum::http::Method::POST)
            .uri(crate::callers::endpoints::QUEUECOVERART)
            .header(axum::http::header::CONTENT_TYPE, content_type)
            .header(axum::http::header::AUTHORIZATION, bearer_auth().await)
            .body(axum::body::Body::from_stream(body))
            .unwrap();

        // Send request
        app.clone().oneshot(req).await
    }

    async fn queue_metadata_req(
        app: &axum::Router,
        song_queue_id: &uuid::Uuid,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let payload = payload_data::queue_metadata_payload_data(&song_queue_id).await;

        let req = axum::http::Request::builder()
            .method(axum::http::Method::POST)
            .uri(crate::callers::endpoints::QUEUEMETADATA)
            .header(axum::http::header::CONTENT_TYPE, "application/json")
            .header(axum::http::header::AUTHORIZATION, bearer_auth().await)
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
            .header(axum::http::header::AUTHORIZATION, bearer_auth().await)
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
            .header(axum::http::header::AUTHORIZATION, bearer_auth().await)
            .body(axum::body::Body::from(payload.to_string()))
            .unwrap();
        app.clone().oneshot(req).await
    }

    async fn create_song_req(
        app: &axum::Router,
        song_queue_id: &uuid::Uuid,
        user_id: &uuid::Uuid,
    ) -> Result<axum::response::Response, std::convert::Infallible> {
        let payload = payload_data::create_song(song_queue_id, user_id).await;

        let req = axum::http::Request::builder()
            .method(axum::http::Method::POST)
            .uri(crate::callers::endpoints::CREATESONG)
            .header(axum::http::header::CONTENT_TYPE, "application/json")
            .header(axum::http::header::AUTHORIZATION, bearer_auth().await)
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
            "status": crate::repo::queue::song::status::READY
        });

        let req = axum::http::Request::builder()
            .method(axum::http::Method::PATCH)
            .uri(crate::callers::endpoints::QUEUESONG)
            .header(axum::http::header::CONTENT_TYPE, "application/json")
            .header(axum::http::header::AUTHORIZATION, bearer_auth().await)
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
            .header(axum::http::header::AUTHORIZATION, bearer_auth().await)
            .body(axum::body::Body::empty())
            .unwrap();

        app.clone().oneshot(req).await
    }

    mod sequence_flow {
        // Flow for queueing song
        pub async fn queue_song_flow(
            app: &axum::Router,
        ) -> Result<(axum::response::Response, uuid::Uuid), std::convert::Infallible> {
            match super::song_queue_req(&app).await {
                Ok(response) => {
                    let resp =
                        super::get_resp_data::<crate::callers::song::response::Response>(response)
                            .await;
                    assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                    assert_eq!(false, resp.data[0].is_nil(), "Should not be empty");
                    let song_queue_id = resp.data[0];
                    assert_eq!(false, song_queue_id.is_nil(), "Should not be empty");

                    let user_id = super::TEST_USER_ID;

                    match super::song_queue_link_req(&app, &song_queue_id, &user_id).await {
                        Ok(response) => {
                            let resp = super::get_resp_data::<
                                crate::callers::song::response::link_user_id::Response,
                            >(response)
                            .await;
                            assert_eq!(
                                false,
                                resp.data.is_empty(),
                                "The response should not be empty"
                            );

                            match super::queue_metadata_req(&app, &song_queue_id).await {
                                Ok(response) => {
                                    let resp = super::get_resp_data::<
                                        crate::callers::song::response::Response,
                                    >(response)
                                    .await;
                                    assert_eq!(false, resp.data.is_empty(), "Should not be empty");

                                    let id = resp.data[0];

                                    match super::fetch_metadata_queue_req(&app, &id).await {
                                        Ok(response) => Ok((response, user_id)),
                                        Err(err) => Err(err),
                                    }
                                }
                            }
                        }
                        Err(err) => Err(err),
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
                Ok((song_response, user_id)) => {
                    let resp = super::get_resp_data::<
                        crate::callers::metadata::response::fetch_metadata::Response,
                    >(song_response)
                    .await;
                    assert_eq!(false, resp.data.is_empty(), "Data should not be empty");
                    let song_queue_id = resp.data[0].song_queue_id;

                    match super::create_song_req(&app, &song_queue_id, &user_id).await {
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

    // TODO: Put this in a util module
    pub async fn resp_to_bytes(
        response: axum::response::Response,
    ) -> Result<axum::body::Bytes, axum::Error> {
        axum::body::to_bytes(response.into_body(), usize::MAX).await
    }

    // TODO: Put this in a util module
    pub async fn get_resp_data<Data>(response: axum::response::Response) -> Data
    where
        Data: for<'a> serde::Deserialize<'a>,
    {
        let body = resp_to_bytes(response).await.unwrap();
        serde_json::from_slice(&body).unwrap()
    }

    pub async fn format_url_with_value(endpoint: &str, value: &uuid::Uuid) -> String {
        let last = endpoint.len() - 5;
        format!("{}/{value}", &endpoint[0..last])
    }

    // TODO: Change the name of the function to be more expressive and put into it's own module
    pub mod payload_data {
        pub async fn queue_metadata_payload_data(song_queue_id: &uuid::Uuid) -> serde_json::Value {
            serde_json::json!(
            {
                    "song_queue_id": song_queue_id,
                    "album" : "I",
                    "album_artist" : "Kuoth",
                    "artist" : "Kuoth",
                    "disc" : 1,
                    "disc_count" : 1,
                    "duration" : 139,
                    "genre" : "Alternative Hip-Hop",
                    "title" : "Hypocrite Like The Rest",
                    "track" : 1,
                    "track_count" : 9,
                    "year" : 2020
            })
        }

        pub async fn create_song(
            song_queue_id: &uuid::Uuid,
            user_id: &uuid::Uuid,
        ) -> serde_json::Value {
            serde_json::json!({
                "title" : "Hypocrite Like The Rest",
                "artist" : "Kuoth",
                "album_artist": "Kuoth",
                "album": "I",
                "genre" : "Alternative Hip-Hop",
                "date": "2020-01-01",
                "track": 1,
                "disc": 1,
                "track_count": 9,
                "disc_count": 1,
                "duration": 139,
                "audio_type": "flac",
                "user_id": user_id,
                "song_queue_id": song_queue_id
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

    #[tokio::test]
    async fn test_song_queue_link_user_id() {
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
                    get_resp_data::<crate::callers::song::response::Response>(response).await;
                assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                assert_eq!(false, resp.data[0].is_nil(), "Should not be empty");

                let song_queue_id = &resp.data[0];
                let user_id = TEST_USER_ID;
                println!("User Id: {user_id:?}");

                match song_queue_link_req(&app, &song_queue_id, &user_id).await {
                    Ok(response) => {
                        let resp = get_resp_data::<
                            crate::callers::song::response::link_user_id::Response,
                        >(response)
                        .await;
                        let collected_user_id = &resp.data[0];

                        assert!(
                            !collected_user_id.is_nil(),
                            "Collected user id should not be nil {collected_user_id:?}"
                        );
                        assert_eq!(
                            user_id, *collected_user_id,
                            "User Id is different. First {user_id:?} Second {collected_user_id:?}"
                        );
                    }
                    Err(err) => {
                        assert!(
                            false,
                            "Error: {err:?} songQueue Id {song_queue_id:?} user id {user_id:?}"
                        );
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

                let old = crate::repo::queue::song::status::PENDING;
                let target_status = crate::repo::queue::song::status::READY;

                match update_song_queue_status_req(&app, &song_queue_id).await {
                    Ok(response) => {
                        let resp = get_resp_data::<
                            crate::callers::song::response::update_status::Response,
                        >(response)
                        .await;
                        assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                        let changed_status = &resp.data[0];

                        assert_eq!(*old, changed_status.old_status, "Old status does not match");
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
                            assert_eq!(false, bytes.is_empty(), "Queued data should not be empty");

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

                            let raw_uri = String::from(crate::callers::endpoints::QUEUESONGUPDATE);
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
                                        .header(
                                            axum::http::header::AUTHORIZATION,
                                            bearer_auth().await,
                                        )
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
                                    assert_eq!(false, resp.data.is_empty(), "Should not be empty");

                                    let updated_song_queued_id = resp.data[0];
                                    assert_eq!(
                                        updated_song_queued_id, *id,
                                        "Song queue Id should match, but they don't. {:?} {:?}",
                                        updated_song_queued_id, id
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
                            assert_eq!(false, bytes.is_empty(), "Queued data should not be empty");
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

                let old = crate::repo::queue::song::status::PENDING;
                let done = crate::repo::queue::song::status::READY;

                match update_song_queue_status_req(&app, &song_queue_id).await {
                    Ok(response) => {
                        let resp = get_resp_data::<
                            crate::callers::song::response::update_status::Response,
                        >(response)
                        .await;
                        assert_eq!(false, resp.data.is_empty(), "Should not be empty");
                        let changed_status = &resp.data[0];

                        assert_eq!(*old, changed_status.old_status, "Old status does not match");
                        assert_eq!(done, changed_status.new_status, "New status does not match");
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
            Ok((response, _user_id)) => {
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
                                            .header(
                                                axum::http::header::AUTHORIZATION,
                                                bearer_auth().await,
                                            )
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
            Ok((response, user_id)) => {
                let resp = get_resp_data::<
                    crate::callers::metadata::response::fetch_metadata::Response,
                >(response)
                .await;
                assert_eq!(false, resp.data.is_empty(), "Data should not be empty");
                let song_q_id = resp.data[0].song_queue_id;

                match create_song_req(&app, &song_q_id, &user_id).await {
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
            Ok((response, user_id)) => {
                let resp = get_resp_data::<
                    crate::callers::metadata::response::fetch_metadata::Response,
                >(response)
                .await;
                assert_eq!(false, resp.data.is_empty(), "Data should not be empty");
                let song_queue_id = resp.data[0].song_queue_id;

                match create_song_req(&app, &song_queue_id, &user_id).await {
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
            Ok((response, user_id)) => {
                let resp = get_resp_data::<
                    crate::callers::metadata::response::fetch_metadata::Response,
                >(response)
                .await;
                assert_eq!(false, resp.data.is_empty(), "Data should not be empty");
                let song_q_id = resp.data[0].song_queue_id;

                match create_song_req(&app, &song_q_id, &user_id).await {
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
                                    .header(axum::http::header::AUTHORIZATION, bearer_auth().await)
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
            Ok((response, user_id)) => {
                let resp = get_resp_data::<
                    crate::callers::metadata::response::fetch_metadata::Response,
                >(response)
                .await;
                assert_eq!(false, resp.data.is_empty(), "Data should not be empty");
                let song_queue_id = resp.data[0].song_queue_id;

                match create_song_req(&app, &song_queue_id, &user_id).await {
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
                                            .header(
                                                axum::http::header::AUTHORIZATION,
                                                bearer_auth().await,
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

    pub mod zzz_after_song_queue {
        use futures::StreamExt;
        use tower::ServiceExt;

        #[tokio::test]
        async fn test_get_songs() {
            let tm_pool = super::db_mgr::get_pool().await.unwrap();
            let db_name = super::db_mgr::generate_db_name().await;

            match super::db_mgr::create_database(&tm_pool, &db_name).await {
                Ok(_) => {
                    println!("Success");
                }
                Err(err) => {
                    assert!(false, "Error: {:?}", err);
                }
            }

            let pool = super::db_mgr::connect_to_db(&db_name).await.unwrap();
            super::db_mgr::migrations(&pool).await;

            let app = super::init::app(pool).await;

            let (id, _, _, _) = test_data::song_id().await.unwrap();

            let uri = format!("{}?id={id}", crate::callers::endpoints::GETSONGS);

            match app
                .clone()
                .oneshot(
                    axum::http::Request::builder()
                        .method(axum::http::Method::GET)
                        .uri(uri)
                        .header(axum::http::header::CONTENT_TYPE, "application/json")
                        .header(
                            axum::http::header::AUTHORIZATION,
                            super::bearer_auth().await,
                        )
                        .body(axum::body::Body::empty())
                        .unwrap(),
                )
                .await
            {
                Ok(response) => {
                    let resp = super::get_resp_data::<
                        crate::callers::song::response::get_songs::Response,
                    >(response)
                    .await;
                    assert_eq!(false, resp.data.is_empty(), "Should not be empty");

                    let song = resp.data[0].clone();
                    assert_eq!(id, song.id, "Id does not match {song:?}");
                }
                Err(err) => {
                    assert!(false, "Error: {err:?}");
                }
            }

            let _ = super::db_mgr::drop_database(&tm_pool, &db_name).await;
        }

        #[tokio::test]
        async fn test_get_coverart() {
            let tm_pool = super::db_mgr::get_pool().await.unwrap();
            let db_name = super::db_mgr::generate_db_name().await;

            match super::db_mgr::create_database(&tm_pool, &db_name).await {
                Ok(_) => {
                    println!("Success");
                }
                Err(err) => {
                    assert!(false, "Error: {:?}", err);
                }
            }

            let pool = super::db_mgr::connect_to_db(&db_name).await.unwrap();
            super::db_mgr::migrations(&pool).await;

            let app = super::init::app(pool).await;

            let id = test_data::coverart_id().await.unwrap();

            let uri = format!("{}?id={id}", crate::callers::endpoints::GETCOVERART);

            match app
                .clone()
                .oneshot(
                    axum::http::Request::builder()
                        .method(axum::http::Method::GET)
                        .uri(uri)
                        .header(axum::http::header::CONTENT_TYPE, "application/json")
                        .header(
                            axum::http::header::AUTHORIZATION,
                            super::bearer_auth().await,
                        )
                        .body(axum::body::Body::empty())
                        .unwrap(),
                )
                .await
            {
                Ok(response) => {
                    let resp = super::get_resp_data::<
                        crate::callers::coverart::response::get_coverart::Response,
                    >(response)
                    .await;
                    assert_eq!(false, resp.data.is_empty(), "Should not be empty");

                    let coverart = resp.data[0].clone();
                    assert_eq!(id, coverart.id, "Id does not match {coverart:?}");
                }
                Err(err) => {
                    assert!(false, "Error: {err:?}");
                }
            }

            let _ = super::db_mgr::drop_database(&tm_pool, &db_name).await;
        }

        pub mod test_data {
            pub async fn song_id() -> Result<(uuid::Uuid, String, String, String), uuid::Error> {
                match uuid::Uuid::parse_str("44cf7940-34ff-489f-9124-d0ec90a55af9") {
                    Ok(id) => Ok((
                        id,
                        String::from("tests/I/"),
                        String::from("track01.flac"),
                        String::from("tests/I/Coverart-1.jpg"),
                    )),
                    Err(err) => Err(err),
                }
            }

            pub async fn other_song_id()
            -> Result<(uuid::Uuid, (String, String), (String, String)), uuid::Error> {
                match uuid::Uuid::parse_str("94cf7940-34ff-489f-9124-d0ec90a55af4") {
                    Ok(id) => Ok((
                        id,
                        (String::from("tests/I/"), String::from("track02.flac")),
                        (String::from("tests/I/"), String::from("Coverart-2.jpg")),
                    )),
                    Err(err) => Err(err),
                }
            }

            pub async fn coverart_id() -> Result<uuid::Uuid, uuid::Error> {
                uuid::Uuid::parse_str("996122cd-5ae9-4013-9934-60768d3006ed")
            }
        }

        #[tokio::test]
        async fn test_stream_song() {
            let tm_pool = super::db_mgr::get_pool().await.unwrap();
            let db_name = super::db_mgr::generate_db_name().await;

            match super::db_mgr::create_database(&tm_pool, &db_name).await {
                Ok(_) => {
                    println!("Success");
                }
                Err(err) => {
                    assert!(false, "Error: {:?}", err);
                }
            }

            let pool = super::db_mgr::connect_to_db(&db_name).await.unwrap();
            super::db_mgr::migrations(&pool).await;

            let app = super::init::app(pool).await;

            let (id, _, _, _) = test_data::song_id().await.unwrap();

            let my_url = crate::callers::endpoints::STREAMSONG;
            let last = my_url.len() - 5;
            let uri = format!("{}/{id}", &my_url[0..last]);

            match app
                .clone()
                .oneshot(
                    axum::http::Request::builder()
                        .method(axum::http::Method::GET)
                        .uri(&uri)
                        .header(
                            axum::http::header::AUTHORIZATION,
                            super::bearer_auth().await,
                        )
                        .body(axum::body::Body::empty())
                        .unwrap(),
                )
                .await
            {
                Ok(response) => {
                    let e = response.into_body();
                    let mut data = e.into_data_stream();
                    while let Some(chunk) = data.next().await {
                        match chunk {
                            Ok(_data) => {}
                            Err(err) => {
                                assert!(false, "Error: {err:?}");
                            }
                        }
                    }
                }
                Err(err) => {
                    assert!(false, "Error: {err:?}");
                }
            }

            let _ = super::db_mgr::drop_database(&tm_pool, &db_name).await;
        }

        #[tokio::test]
        async fn test_download_song() {
            let tm_pool = super::db_mgr::get_pool().await.unwrap();
            let db_name = super::db_mgr::generate_db_name().await;

            match super::db_mgr::create_database(&tm_pool, &db_name).await {
                Ok(_) => {
                    println!("Success");
                }
                Err(err) => {
                    assert!(false, "Error: {:?}", err);
                }
            }

            let pool = super::db_mgr::connect_to_db(&db_name).await.unwrap();
            super::db_mgr::migrations(&pool).await;

            let app = super::init::app(pool).await;

            let (id, _, _, _) = test_data::song_id().await.unwrap();

            let uri =
                super::format_url_with_value(crate::callers::endpoints::DOWNLOADSONG, &id).await;

            match app
                .clone()
                .oneshot(
                    axum::http::Request::builder()
                        .method(axum::http::Method::GET)
                        .uri(&uri)
                        .header(
                            axum::http::header::AUTHORIZATION,
                            super::bearer_auth().await,
                        )
                        .body(axum::body::Body::empty())
                        .unwrap(),
                )
                .await
            {
                Ok(response) => {
                    let e = response.into_body();
                    let mut data = e.into_data_stream();
                    while let Some(chunk) = data.next().await {
                        match chunk {
                            Ok(_data) => {}
                            Err(err) => {
                                assert!(false, "Error: {err:?}");
                            }
                        }
                    }
                }
                Err(err) => {
                    assert!(false, "Error: {err:?}");
                }
            }

            let _ = super::db_mgr::drop_database(&tm_pool, &db_name).await;
        }

        #[tokio::test]
        async fn test_download_coverart() {
            let tm_pool = super::db_mgr::get_pool().await.unwrap();
            let db_name = super::db_mgr::generate_db_name().await;

            match super::db_mgr::create_database(&tm_pool, &db_name).await {
                Ok(_) => {
                    println!("Success");
                }
                Err(err) => {
                    assert!(false, "Error: {:?}", err);
                }
            }

            let pool = super::db_mgr::connect_to_db(&db_name).await.unwrap();
            super::db_mgr::migrations(&pool).await;

            let app = super::init::app(pool).await;

            let id = test_data::coverart_id().await.unwrap();

            let uri =
                super::format_url_with_value(crate::callers::endpoints::DOWNLOADCOVERART, &id)
                    .await;

            match app
                .clone()
                .oneshot(
                    axum::http::Request::builder()
                        .method(axum::http::Method::GET)
                        .uri(&uri)
                        .header(
                            axum::http::header::AUTHORIZATION,
                            super::bearer_auth().await,
                        )
                        .body(axum::body::Body::empty())
                        .unwrap(),
                )
                .await
            {
                Ok(response) => {
                    let e = response.into_body();
                    let mut data = e.into_data_stream();
                    while let Some(chunk) = data.next().await {
                        match chunk {
                            Ok(_data) => {}
                            Err(err) => {
                                assert!(false, "Error: {err:?}");
                            }
                        }
                    }
                }
                Err(err) => {
                    assert!(false, "Error: {err:?}");
                }
            }

            let _ = super::db_mgr::drop_database(&tm_pool, &db_name).await;
        }

        async fn get_test_data(
            song_directory: &String,
            song_filename: &String,
            coverart_directory: &String,
            coverart_filename: &String,
        ) -> Result<(Vec<u8>, Vec<u8>), std::io::Error> {
            let song = icarus_models::song::Song {
                directory: song_directory.clone(),
                filename: song_filename.clone(),
                ..Default::default()
            };

            let coverart = icarus_models::coverart::CoverArt {
                directory: coverart_directory.clone(),
                filename: coverart_filename.clone(),
                ..Default::default()
            };

            match icarus_models::song::io::to_data(&song) {
                Ok(song_data) => match icarus_models::coverart::io::to_data(&coverart) {
                    Ok(coverart_data) => Ok((song_data, coverart_data)),
                    Err(err) => Err(err),
                },
                Err(err) => Err(err),
            }
        }

        async fn save_test_again(
            song_directory: &String,
            song_filename: &String,
            song_data: Vec<u8>,
            coverart_directory: &String,
            coverart_filename: &String,
            coverart_data: Vec<u8>,
        ) -> Result<(), std::io::Error> {
            let song = icarus_models::song::Song {
                directory: song_directory.clone(),
                filename: song_filename.clone(),
                data: song_data,
                ..Default::default()
            };

            let coverart = icarus_models::coverart::CoverArt {
                directory: coverart_directory.clone(),
                filename: coverart_filename.clone(),
                data: coverart_data,
                ..Default::default()
            };

            match song.save_to_filesystem() {
                Ok(_) => match coverart.save_to_filesystem() {
                    Ok(_) => Ok(()),
                    Err(err) => Err(err),
                },
                Err(err) => Err(err),
            }
        }

        #[tokio::test]
        async fn test_last_delete_song() {
            let tm_pool = super::db_mgr::get_pool().await.unwrap();
            let db_name = super::db_mgr::generate_db_name().await;

            match super::db_mgr::create_database(&tm_pool, &db_name).await {
                Ok(_) => {
                    println!("Success");
                }
                Err(err) => {
                    assert!(false, "Error: {:?}", err);
                }
            }

            let pool = super::db_mgr::connect_to_db(&db_name).await.unwrap();
            super::db_mgr::migrations(&pool).await;

            let app = super::init::app(pool).await;

            let (id, (song_directory, song_filename), (cover_directory, cover_filename)) =
                test_data::other_song_id().await.unwrap();
            let (song_data, coverart_data) = get_test_data(
                &song_directory,
                &song_filename,
                &cover_directory,
                &cover_filename,
            )
            .await
            .unwrap();

            let uri =
                super::format_url_with_value(crate::callers::endpoints::DELETESONG, &id).await;

            match app
                .clone()
                .oneshot(
                    axum::http::Request::builder()
                        .method(axum::http::Method::DELETE)
                        .uri(&uri)
                        .header(
                            axum::http::header::AUTHORIZATION,
                            super::bearer_auth().await,
                        )
                        .body(axum::body::Body::empty())
                        .unwrap(),
                )
                .await
            {
                Ok(response) => {
                    let resp = super::get_resp_data::<
                        crate::callers::song::response::delete_song::Response,
                    >(response)
                    .await;
                    assert_eq!(false, resp.data.is_empty(), "Response has no data");

                    let song_and_coverart = &resp.data[0];
                    assert_eq!(
                        id, song_and_coverart.song.id,
                        "Song Ids do not match {id:?} {:?}",
                        song_and_coverart.song.id
                    );

                    match save_test_again(
                        &song_directory,
                        &song_filename,
                        song_data,
                        &cover_directory,
                        &cover_filename,
                        coverart_data,
                    )
                    .await
                    {
                        Ok(_) => {}
                        Err(err) => {
                            assert!(false, "Error: {err:?}");
                        }
                    }
                }
                Err(err) => {
                    assert!(false, "Error: {err:?}");
                }
            }

            let _ = super::db_mgr::drop_database(&tm_pool, &db_name).await;
        }

        #[tokio::test]
        async fn test_get_all_songs() {
            let tm_pool = super::db_mgr::get_pool().await.unwrap();
            let db_name = super::db_mgr::generate_db_name().await;

            match super::db_mgr::create_database(&tm_pool, &db_name).await {
                Ok(_) => {
                    println!("Success");
                }
                Err(err) => {
                    assert!(false, "Error: {:?}", err);
                }
            }

            let pool = super::db_mgr::connect_to_db(&db_name).await.unwrap();
            super::db_mgr::migrations(&pool).await;

            let app = super::init::app(pool).await;

            match app
                .clone()
                .oneshot(
                    axum::http::Request::builder()
                        .method(axum::http::Method::GET)
                        .uri(crate::callers::endpoints::GETALLSONGS)
                        .header(axum::http::header::CONTENT_TYPE, "application/json")
                        .header(
                            axum::http::header::AUTHORIZATION,
                            super::bearer_auth().await,
                        )
                        .body(axum::body::Body::empty())
                        .unwrap(),
                )
                .await
            {
                Ok(response) => {
                    let resp = super::get_resp_data::<
                        crate::callers::song::response::get_songs::Response,
                    >(response)
                    .await;
                    assert_eq!(false, resp.data.is_empty(), "Should not be empty");

                    let songs = &resp.data;
                    assert_eq!(
                        2,
                        songs.len(),
                        "Returned song count does not match. Returned song count {:?} song count {}",
                        songs.len(),
                        2
                    );
                }
                Err(err) => {
                    assert!(false, "Error: {err:?}");
                }
            }

            let _ = super::db_mgr::drop_database(&tm_pool, &db_name).await;
        }
    }
}
