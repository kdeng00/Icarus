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

    pub mod fetch_queue_song {
        use serde::{Deserialize, Serialize};

        #[derive(Default, Deserialize, Serialize)]
        pub struct Response {
            pub message: String,
            pub data: Vec<crate::callers::song::song_queue::SongQueue>,
        }
    }
}

mod song_queue {
    use sqlx::Row;

    pub mod status {
        pub const PENDING: &str = "pending";
        // Will be used later on
        pub const PROCESSING: &str = "processing";
        pub const _DONE: &str = "done";
    }

    #[derive(Debug, serde::Serialize, sqlx::FromRow)]
    pub struct InsertedData {
        pub id: uuid::Uuid,
    }

    #[derive(Debug, serde::Deserialize, serde::Serialize, sqlx::FromRow)]
    pub struct SongQueue {
        pub id: uuid::Uuid,
        pub filename: String,
        pub status: String,
    }

    pub async fn insert(
        pool: &sqlx::PgPool,
        data: &Vec<u8>,
        filename: &String,
        status: &String,
    ) -> Result<uuid::Uuid, sqlx::Error> {
        let result = sqlx::query(
            r#"
            INSERT INTO "songQueue" (data, filename, status) VALUES($1, $2, $3) RETURNING id;
            "#,
        )
        .bind(data)
        .bind(filename)
        .bind(status)
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

    pub async fn get_most_recent_and_update(pool: &sqlx::PgPool) -> Result<SongQueue, sqlx::Error> {
        let result = sqlx::query(
            r#"
            UPDATE "songQueue"
            SET status = $1
            WHERE id = (
                SELECT id FROM "songQueue"
                WHERE status = $2
                ORDER BY id
                FOR UPDATE SKIP LOCKED
                LIMIT 1
            )
            RETURNING id, filename, status;
            "#,
        )
        .bind(status::PROCESSING)
        .bind(status::PENDING)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error inserting: {}", e);
        });

        match result {
            Ok(row) => Ok(SongQueue {
                id: row
                    .try_get("id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                filename: row
                    .try_get("filename")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                status: row
                    .try_get("status")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
            }),
            Err(_err) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn get_data(pool: &sqlx::PgPool, id: &uuid::Uuid) -> Result<Vec<u8>, sqlx::Error> {
        let result = sqlx::query(
            r#"
            SELECT data FROM "songQueue"
            WHERE id = $1;
            "#,
        )
        .bind(id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error inserting: {}", e);
        });

        match result {
            Ok(row) => {
                let data = row
                    .try_get("data")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap();
                Ok(data)
            }
            Err(_err) => Err(sqlx::Error::RowNotFound),
        }
    }
}

pub mod endpoint {
    use axum::{Json, http::StatusCode, response::IntoResponse};
    use std::io::Write;

    use crate::callers::song::song_queue;

    pub async fn queue_song(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        mut multipart: axum::extract::Multipart,
    ) -> (StatusCode, Json<super::response::Response>) {
        let mut results: Vec<uuid::Uuid> = Vec::new();
        let mut response = super::response::Response::default();

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
            let mut file = std::fs::File::create(&file_name).unwrap();
            file.write_all(&data).unwrap();

            let raw_data: Vec<u8> = data.to_vec();
            let queue_repo = song_queue::insert(
                &pool,
                &raw_data,
                &file_name,
                &song_queue::status::PENDING.to_string(),
            )
            .await
            .unwrap();
            results.push(queue_repo);
        }

        response.data = results;
        response.message = if response.data.is_empty() {
            String::from("Error")
        } else {
            String::from("Success")
        };

        (StatusCode::OK, Json(response))
    }

    pub async fn fetch_queue_song(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
    ) -> (
        StatusCode,
        Json<super::response::fetch_queue_song::Response>,
    ) {
        let mut response = super::response::fetch_queue_song::Response::default();

        match song_queue::get_most_recent_and_update(&pool).await {
            Ok(item) => {
                response.message = String::from("Successful");
                response.data.push(item);
                (StatusCode::OK, Json(response))
            }
            Err(err) => {
                response.message = err.to_string();
                (StatusCode::BAD_REQUEST, Json(response))
            }
        }
    }

    pub async fn download_flac(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Path(id): axum::extract::Path<uuid::Uuid>,
    ) -> (StatusCode, axum::response::Response) {
        println!("Id: {:?}", id);

        match song_queue::get_data(&pool, &id).await {
            Ok(data) => {
                let by = axum::body::Bytes::from(data);
                let mut response = by.into_response();
                let headers = response.headers_mut();
                headers.insert(
                    axum::http::header::CONTENT_TYPE,
                    "audio/flac".parse().unwrap(),
                );
                headers.insert(
                    axum::http::header::CONTENT_DISPOSITION,
                    format!("attachment; filename=\"{}.flac\"", id)
                        .parse()
                        .unwrap(),
                );

                (StatusCode::OK, response)
            }
            Err(_err) => (StatusCode::BAD_REQUEST, axum::response::Response::default()),
        }
    }
}
