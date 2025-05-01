pub mod response {
    #[derive(Debug, Default, serde::Deserialize, serde::Serialize)]
    pub struct Response {
        pub rando: i32,
    }
}

pub mod endpoint {
    // TODO: Implement this
    pub async fn queue(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        mut multipart: axum::extract::Multipart,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::Response>,
    ) {
        let response = super::response::Response::default();

        while let Some(field) = multipart.next_field().await.unwrap() {
            let name = field.name().unwrap().to_string();
            let file_name = field.file_name().unwrap().to_string();
            let content_type = field.content_type().unwrap().to_string();
            let data = field.bytes().await.unwrap();

            println!(
                "Received file '{}' (name = '{}', content-type = '{}', size = {})",
                file_name,
                name,
                content_type,
                data.len()
            );

            // Save the file to disk
            // let mut file = std::fs::File::create(&file_name).unwrap();
            // file.write_all(&data).unwrap();
        }

        (axum::http::StatusCode::OK, axum::Json(response))
    }
}
