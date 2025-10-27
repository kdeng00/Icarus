pub mod request {
    pub mod queue_metadata {
        #[derive(
            Debug, Default, serde::Deserialize, serde::Serialize, sqlx::FromRow, utoipa::ToSchema,
        )]
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
            Debug,
            Default,
            serde::Deserialize,
            serde::Serialize,
            sqlx::FromRow,
            sqlx::Decode,
            utoipa::ToSchema,
        )]
        pub struct Params {
            pub id: Option<uuid::Uuid>,
            pub song_queue_id: Option<uuid::Uuid>,
        }
    }
}

pub mod response {
    pub mod queue_metadata {
        #[derive(Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<uuid::Uuid>,
        }
    }

    pub mod fetch_metadata {
        #[derive(Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<crate::repo::queue::metadata::MetadataQueue>,
        }
    }
}

/// Module for metadata related endpoints
pub mod endpoint {
    use crate::repo::queue as repo_queue;

    /// Endpoint to create queued metadata
    #[utoipa::path(
        post,
        path = super::super::endpoints::QUEUEMETADATA,
        request_body(
            content = super::request::queue_metadata::Request,
            description = "Data required to create queued metadata",
            content_type = "application/json"
        ),
        responses(
            (status = 200, description = "Queued metadata created", body = super::response::queue_metadata::Response),
            (status = 400, description = "Error creating queued metadata", body = super::response::queue_metadata::Response)
        )
    )]
    pub async fn queue_metadata(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::Json(payload): axum::Json<super::request::queue_metadata::Request>,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::queue_metadata::Response>,
    ) {
        let mut results: Vec<uuid::Uuid> = Vec::new();
        let mut response = super::response::queue_metadata::Response::default();
        let meta = payload.to_json_value().await;
        match repo_queue::metadata::insert(&pool, &meta, &payload.song_queue_id).await {
            Ok(metadata_queue_id) => {
                results.push(metadata_queue_id);
                response.data = results;
                response.message = if response.data.is_empty() {
                    String::from("Error")
                } else {
                    String::from(super::super::super::response::SUCCESSFUL)
                };

                (axum::http::StatusCode::OK, axum::Json(response))
            }
            Err(err) => {
                response.message = err.to_string();
                (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
            }
        }
    }

    /// Endpoint to get queued metadata
    #[utoipa::path(
        get,
        path = super::super::endpoints::QUEUEMETADATA,
        params(
            ("id" = uuid::Uuid, Path, description = "Id of queued metadata"),
            ("song_queue_id" = uuid::Uuid, Path, description = "Id of queued song")
            ),
        responses(
            (status = 200, description = "Queued metadata retrieved", body = super::response::fetch_metadata::Response),
            (status = 400, description = "Error retrieving queued metadata", body = super::response::fetch_metadata::Response)
        )
    )]
    pub async fn fetch_metadata(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Query(params): axum::extract::Query<super::request::fetch_metadata::Params>,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::fetch_metadata::Response>,
    ) {
        let mut response = super::response::fetch_metadata::Response::default();

        match params.id {
            Some(id) => {
                println!("Something works {id}");

                match repo_queue::metadata::get_with_id(&pool, &id).await {
                    Ok(item) => {
                        response.message = String::from("Successful");
                        response.data.push(item);
                        (axum::http::StatusCode::OK, axum::Json(response))
                    }
                    Err(err) => {
                        response.message = err.to_string();
                        (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                    }
                }
            }
            _ => match params.song_queue_id {
                Some(song_queue_id) => {
                    println!("Song queue Id is probably not nil");
                    match repo_queue::metadata::get_with_song_queue_id(&pool, &song_queue_id).await
                    {
                        Ok(item) => {
                            response.message = String::from("Successful");
                            response.data.push(item);
                            (axum::http::StatusCode::OK, axum::Json(response))
                        }
                        Err(err) => {
                            response.message = err.to_string();
                            (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                        }
                    }
                }
                None => {
                    println!("What is going on?");
                    (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                }
            },
        }
    }
}
