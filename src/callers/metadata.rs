pub mod request {
    use serde::{Deserialize, Serialize};

    #[derive(Default, Deserialize, Serialize)]
    pub struct Request {
        pub id: uuid::Uuid,
        pub album: String,
        pub album_artist: String,
        pub artist: String,
        pub disc: i32,
        pub disc_count: i32,
        pub duration: i64,
        pub genre: String,
        pub title: String,
        pub track: i32,
        pub track_count: i32,
        pub year: i32,
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

mod metadata_queue {
    use sqlx::Row;

    #[derive(Debug, serde::Serialize, sqlx::FromRow)]
    pub struct InsertedData {
        pub id: uuid::Uuid,
    }

    pub async fn insert(
        pool: &sqlx::PgPool,
        metadata: &serde_json::Value,
        song_queue_id: &uuid::Uuid,
    ) -> Result<uuid::Uuid, sqlx::Error> {
        let result = sqlx::query(
            r#"
            INSERT INTO "metadataQueue" (metadata, song_queue_id) VALUES($1, $2) RETURNING id;
            "#,
        )
        .bind(metadata)
        .bind(song_queue_id)
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
    use axum::{Json, http::StatusCode};

    pub async fn queue_metadata(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        Json(payload): Json<super::request::Request>,
    ) -> (StatusCode, Json<super::response::Response>) {
        let mut results: Vec<uuid::Uuid> = Vec::new();
        let mut response = super::response::Response::default();
        let meta = serde_json::json!(
            {
                  "id": &payload.id,
                "album": &payload.album,
                "album_artist": &payload.album_artist,
                "genre": &payload.genre,
                "year": &payload.year,
                "track_count": &payload.track_count,
                "disc_count": &payload.disc_count,
                "title": &payload.title,
                "artist": &payload.artist,
                "disc": &payload.disc,
                "track": &payload.track,
                "duration": &payload.duration,
            });
        match super::metadata_queue::insert(&pool, &meta, &payload.id).await {
            Ok(id) => {
                results.push(id);
                response.data = results;
                response.message = if response.data.is_empty() {
                    String::from("Error")
                } else {
                    String::from("Success")
                };

                (StatusCode::OK, Json(response))
            }
            Err(err) => {
                response.message = err.to_string();
                (StatusCode::BAD_REQUEST, Json(response))
            }
        }
    }
}
