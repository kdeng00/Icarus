use axum::{
    Json, Router,
    http::StatusCode,
    routing::{get, post},
};
use serde::{Deserialize, Serialize};

#[tokio::main]
async fn main() {
    // initialize tracing
    tracing_subscriber::fmt::init();

    // build our application with a route
    let app = Router::new()
        // `GET /` goes to `root`
        .route("/", get(root));
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
