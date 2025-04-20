use axum::{
    // Json,
    Router,
    // http::StatusCode,
    // routing::{get, post},
    routing::{get, post},
};
// use serde::{Deserialize, Serialize};

pub mod callers;

mod db {

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

    // use crate::{connection_settings, keys};

    pub async fn create_pool() -> Result<sqlx::PgPool, sqlx::Error> {
        let database_url = get_db_url().await;
        println!("Database url: {:?}", database_url);

        PgPoolOptions::new()
            .max_connections(connection_settings::MAXCONN)
            .connect(&database_url)
            .await
    }

    async fn get_db_url() -> String {
        // #[cfg(debug_assertions)] // Example: Only load .env in debug builds
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
    let app = Router::new()
        // `GET /` goes to `root`
        .route("/", get(root))
        .route(callers::endpoints::QUEUESONG, post(callers::song::endpoint::queue_song))
        .layer(axum::Extension(pool));
    // `POST /users` goes to `create_user`
    // .route("/users", post(create_user));

    // run our app with hyper, listening globally on port 3000
    let listener = tokio::net::TcpListener::bind(get_full()).await.unwrap();
    axum::serve(listener, app).await.unwrap();
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

// basic handler that responds with a static string
async fn root() -> &'static str {
    "Hello, World!"
}
