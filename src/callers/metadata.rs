// TODO: Explicitly make this module target queueing a song's metadata
pub mod request {

    pub mod queue_metadata {
        use serde::{Deserialize, Serialize};

        #[derive(Debug, Default, Deserialize, Serialize, sqlx::FromRow)]
        pub struct Request {
            pub song_queue_id: uuid::Uuid,
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
                    "song_queue_id": &self.song_queue_id,
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
    }

    pub mod fetch_metadata {
        #[derive(
            Debug, Default, serde::Deserialize, serde::Serialize, sqlx::FromRow, sqlx::Decode,
        )]
        pub struct Params {
            pub id: Option<uuid::Uuid>,
            pub song_queue_id: Option<uuid::Uuid>,
        }
    }
}

pub mod response {

    pub mod queue_metadata {
        use serde::{Deserialize, Serialize};

        #[derive(Default, Deserialize, Serialize)]
        pub struct Response {
            pub message: String,
            pub data: Vec<uuid::Uuid>,
        }
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
        pub metadata: serde_json::Value,
        #[serde(with = "time::serde::rfc3339")]
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
            eprintln!("Error inserting: {e}");
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
            eprintln!("Error inserting: {e}");
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

    pub async fn get_with_id(
        pool: &sqlx::PgPool,
        id: &uuid::Uuid,
    ) -> Result<MetadataQueue, sqlx::Error> {
        let result = sqlx::query(
            r#"
            SELECT id, metadata, created_at, song_queue_id FROM "metadataQueue" WHERE id = $1;
            "#,
        )
        .bind(id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error inserting: {e}");
        });

        match result {
            Ok(row) => {
                let data: serde_json::Value = row
                    .try_get("metadata")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap();
                Ok(MetadataQueue {
                    id: row
                        .try_get("id")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    metadata: data,
                    created_at: row
                        .try_get("created_at")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    song_queue_id: row
                        .try_get("song_queue_id")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                })
            }
            Err(_err) => Err(sqlx::Error::RowNotFound),
        }
    }
}

pub mod endpoint {
    use axum::{Json, http::StatusCode};

    #[utoipa::path(
        post,
        path = "/api/v2/song/queue/link",
        request_body(
            content = super::request::link_user_id::Request,
            description = "User Id and queued song id",
            content_type = "application/json"
            ),
        responses(
            (status = 200, description = "Queued song linked", body = super::response::link_user_id::Response),
            (status = 400, description = "Linkage failed", body = super::response::link_user_id::Response)
        )
    )]
    pub async fn queue_metadata(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        Json(payload): Json<super::request::queue_metadata::Request>,
    ) -> (StatusCode, Json<super::response::queue_metadata::Response>) {
        let mut results: Vec<uuid::Uuid> = Vec::new();
        let mut response = super::response::queue_metadata::Response::default();
        let meta = payload.to_json_value().await;
        match super::metadata_queue::insert(&pool, &meta, &payload.song_queue_id).await {
            Ok(metadata_queue_id) => {
                results.push(metadata_queue_id);
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

    #[utoipa::path(
        post,
        path = "/api/v2/song/queue/link",
        request_body(
            content = super::request::link_user_id::Request,
            description = "User Id and queued song id",
            content_type = "application/json"
            ),
        responses(
            (status = 200, description = "Queued song linked", body = super::response::link_user_id::Response),
            (status = 400, description = "Linkage failed", body = super::response::link_user_id::Response)
        )
    )]
    pub async fn fetch_metadata(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Query(params): axum::extract::Query<super::request::fetch_metadata::Params>,
    ) -> (StatusCode, Json<super::response::fetch_metadata::Response>) {
        let mut response = super::response::fetch_metadata::Response::default();

        match params.id {
            Some(id) => {
                println!("Something works {id}");

                match super::metadata_queue::get_with_id(&pool, &id).await {
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
            _ => match params.song_queue_id {
                Some(song_queue_id) => {
                    println!("Song queue Id is probably not nil");
                    match super::metadata_queue::get_with_song_queue_id(&pool, &song_queue_id).await
                    {
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
                None => {
                    println!("What is going on?");
                    (StatusCode::BAD_REQUEST, Json(response))
                }
            },
        }
    }
}
