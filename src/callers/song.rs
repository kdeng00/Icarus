pub mod request {
    use serde::{Deserialize, Serialize};

    #[derive(Default, Deserialize, Serialize)]
    pub struct Request {
        pub message: String,
    }

}

pub mod response {
    use serde::{Deserialize, Serialize};

    #[derive(Default, Deserialize, Serialize)]
    pub struct Response {
        pub message: String,
        pub data: Vec<uuid::Uuid>,
    }
}

pub mod endpoint {
    use axum::{http::{StatusCode}, Json};

    pub async fn queue_song(
        Json(payload): Json<super::request::Request>,
    ) -> (StatusCode, Json<super::response::Response>) {
        (StatusCode::OK, Json(super::response::Response::default()))
    }

}
