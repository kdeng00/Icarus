pub mod request {

    pub mod link {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize)]
        pub struct Request {
            pub coverart_id: uuid::Uuid,
            pub song_queue_id: uuid::Uuid,
        }
    }
}

pub mod response {
    #[derive(Debug, Default, serde::Deserialize, serde::Serialize)]
    pub struct Response {
        pub message: String,
        pub data: Vec<uuid::Uuid>,
    }

    pub mod link {
        #[derive(Debug, serde::Deserialize, serde::Serialize)]
        pub struct Id {
            pub coverart_id: uuid::Uuid,
            pub song_queue_id: uuid::Uuid,
        }

        #[derive(Debug, Default, serde::Deserialize, serde::Serialize)]
        pub struct Response {
            pub message: String,
            pub data: Vec<Id>,
        }
    }
}

mod db {
    use sqlx::Row;

    pub async fn insert(pool: &sqlx::PgPool, data: &Vec<u8>) -> Result<uuid::Uuid, sqlx::Error> {
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

    pub async fn update(
        pool: &sqlx::PgPool,
        coverart_id: &uuid::Uuid,
        song_queue_id: &uuid::Uuid,
    ) -> Result<i32, sqlx::Error> {
        let result = sqlx::query(
            r#"
            UPDATE "coverartQueue" SET song_queue_id = $1 WHERE id = $2;
            "#,
        )
        .bind(song_queue_id)
        .bind(coverart_id)
        .execute(pool)
        .await;

        match result {
            Ok(_) => Ok(0),
            Err(_err) => Err(sqlx::Error::RowNotFound),
        }
    }
}

pub mod endpoint {
    pub async fn queue(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        mut multipart: axum::extract::Multipart,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::Response>,
    ) {
        let mut response = super::response::Response::default();

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
            }
            Ok(None) => (axum::http::StatusCode::BAD_REQUEST, axum::Json(response)),
            Err(err) => {
                response.message = err.to_string();
                (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
            }
        }
    }

    pub async fn link(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::Json(payload): axum::Json<super::request::link::Request>,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::link::Response>,
    ) {
        let mut response = super::response::link::Response::default();
        let coverart_id = payload.coverart_id;
        let song_queue_id = payload.song_queue_id;

        match super::db::update(&pool, &coverart_id, &song_queue_id).await {
            Ok(_o) => {
                response.data.push(super::response::link::Id {
                    song_queue_id: song_queue_id,
                    coverart_id: coverart_id,
                });

                (axum::http::StatusCode::OK, axum::Json(response))
            }
            Err(err) => {
                response.message = err.to_string();
                (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
            }
        }
    }
}
