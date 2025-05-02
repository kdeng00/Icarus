pub mod response {
    #[derive(Debug, Default, serde::Deserialize, serde::Serialize)]
    pub struct Response {
        pub message: String,
        pub data: Vec<uuid::Uuid>,
    }
}

mod db {
    use sqlx::Row;

    pub async fn insert(
        pool: &sqlx::PgPool,
        data: &Vec<u8>,
    ) -> Result<uuid::Uuid, sqlx::Error> {
        let result = sqlx::query(
            r#"
            INSERT INTO "coverartQueue" (data) VALUES($1) RETURNING id;
            "#,
        )
        .bind(data)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error inserting: {}", e);
        });

        match result {
            Ok(row) => {
                let id: uuid::Uuid = row
                    .try_get("id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap();
                Ok(id)
            }
            Err(_err) => Err(sqlx::Error::RowNotFound),
        }
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
        let mut response = super::response::Response::default();

        // while let Some(field) = multipart.next_field().await.unwrap() {
        match multipart.next_field().await {
            Ok(Some(field)) => {
            let name = field.name().unwrap().to_string();
            let file_name = field.file_name().unwrap().to_string();
            let content_type = field.content_type().unwrap().to_string();
            let data = field.bytes().await.unwrap();
            let raw_data = data.to_vec();

            println!(
                "Received file '{}' (name = '{}', content-type = '{}', size = {})",
                file_name,
                name,
                content_type,
                data.len()
            );

             match super::db::insert(&pool, &raw_data).await {
                 Ok(id) => {
                     response.message = String::from("Successful");
                     response.data.push(id);
                    (axum::http::StatusCode::OK, axum::Json(response))
                 }
                 Err(err) => {
                     response.message = err.to_string();
                    (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                 }
             }

            // Save the file to disk
            // let mut file = std::fs::File::create(&file_name).unwrap();
            // file.write_all(&data).unwrap();
            }
            Ok(None) => {
                    (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
            }
            Err(err) => {
                     response.message = err.to_string();
                    (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
            }
        }
    }
}
