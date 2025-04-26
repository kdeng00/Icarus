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

    impl Request {
        pub async fn to_json_value(&self) -> serde_json::Value {
            serde_json::json!(
            {
                "id": &self.id,
                "album": &self.album,
                "album_artist": &self.album_artist,
                "genre": &self.genre,
                "year": &self.year,
                "track_count": &self.track_count,
                "disc_count": &self.disc_count,
                "title": &self.title,
                "artist": &self.artist,
                "disc": &self.disc,
                "track": &self.track,
                "duration": &self.duration,
            })
        }
    }

    pub mod fetch_metadata {
        #[derive(serde::Deserialize)]
        pub struct Params {
            pub id: Option<uuid::Uuid>,
            pub song_queue_id: Option<uuid::Uuid>,
        }
    }
}

pub mod response {
    use serde::{Deserialize, Serialize};

    #[derive(Default, Deserialize, Serialize)]
    pub struct Response {
        pub message: String,
        pub data: Vec<uuid::Uuid>,
    }

    pub mod fetch_metadata {
        use serde::{Deserialize, Serialize};

        #[derive(Default, Deserialize, Serialize)]
        pub struct Response {
            pub message: String,
            pub data: Vec<crate::callers::metadata::metadata_queue::MetadataQueue>,
        }
    }
}

pub mod metadata_queue {
    use sqlx::Row;

    #[derive(Debug, serde::Serialize, sqlx::FromRow)]
    pub struct InsertedData {
        pub id: uuid::Uuid,
    }

    #[derive(Debug, serde::Deserialize, serde::Serialize, sqlx::FromRow)]
    pub struct MetadataQueue {
        pub id: uuid::Uuid,
        pub metadata: String,
        pub created_at: time::OffsetDateTime,
        pub song_queue_id: uuid::Uuid,
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

    pub async fn get_with_song_queue_id(
        pool: &sqlx::PgPool,
        song_queue_id: &uuid::Uuid,
    ) -> Result<MetadataQueue, sqlx::Error> {
        let result = sqlx::query(
            r#"
            SELECT * FROM "metadataQueue" WHERE song_queue_id = $1;
            "#,
        )
        .bind(song_queue_id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error inserting: {}", e);
        });

        match result {
            Ok(row) => Ok(MetadataQueue {
                id: row
                    .try_get("id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                metadata: row
                    .try_get("metadata")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                created_at: row
                    .try_get("created_at")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                song_queue_id: row
                    .try_get("song_queue_id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
            }),
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
        let meta = payload.to_json_value().await;
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

    pub async fn fetch_metadata(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Query(params): axum::extract::Query<super::request::fetch_metadata::Params>,
    ) -> (StatusCode, Json<super::response::fetch_metadata::Response>) {
        let mut response = super::response::fetch_metadata::Response::default();
        let song_queue_id: uuid::Uuid = match params.song_queue_id {
            Some(id) => id,
            None => uuid::Uuid::nil(),
        };
        // TODO: Make sure id works as well

        match super::metadata_queue::get_with_song_queue_id(&pool, &song_queue_id).await {
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
}
