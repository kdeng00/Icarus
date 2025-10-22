// TODO: Separate queue and song endpoints
pub mod request {
    use serde::{Deserialize, Serialize};

    #[derive(Default, Deserialize, Serialize)]
    pub struct Request {
        pub message: String,
    }

    pub mod song_queue {
        #[derive(utoipa::ToSchema)]
        pub struct SongQueueRequest {
            /// Filename
            pub file: String,
            #[schema(rename = "type")]
            /// File type. Should be a file and not a value
            pub file_type: String,
            /// Raw data of the flac file
            pub value: Vec<u8>,
        }
    }

    pub mod update_status {
        #[derive(Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Request {
            pub id: uuid::Uuid,
            pub status: String,
        }
    }

    pub mod create_metadata {
        #[derive(Debug, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Request {
            pub title: String,
            pub artist: String,
            pub album_artist: String,
            pub album: String,
            pub genre: String,
            pub date: String,
            pub track: i32,
            pub disc: i32,
            pub track_count: i32,
            pub disc_count: i32,
            pub duration: i32,
            pub audio_type: String,
            pub user_id: uuid::Uuid,
            pub song_queue_id: uuid::Uuid,
        }

        impl Request {
            pub fn is_valid(&self) -> bool {
                !self.title.is_empty()
                    || !self.artist.is_empty()
                    || !self.album_artist.is_empty()
                    || !self.album.is_empty()
                    || !self.genre.is_empty()
                    || !self.date.is_empty()
                    || self.track > 0
                    || self.disc > 0
                    || self.track_count > 0
                    || self.disc_count > 0
                    || self.duration > 0
                    || !self.audio_type.is_empty()
                    || !self.user_id.is_nil()
                    || !self.song_queue_id.is_nil()
            }

            pub fn to_song(&self) -> icarus_models::song::Song {
                icarus_models::song::Song {
                    id: uuid::Uuid::nil(),
                    title: self.title.clone(),
                    artist: self.artist.clone(),
                    album_artist: self.album_artist.clone(),
                    album: self.album.clone(),
                    genre: self.genre.clone(),
                    year: self.date[..3].parse().unwrap(),
                    track: self.track,
                    disc: self.disc,
                    track_count: self.track_count,
                    disc_count: self.disc_count,
                    duration: self.duration,
                    audio_type: self.audio_type.clone(),
                    user_id: self.user_id,
                    ..Default::default()
                }
            }
        }
    }

    pub mod wipe_data_from_song_queue {
        #[derive(Debug, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Request {
            pub song_queue_id: uuid::Uuid,
        }
    }

    pub mod link_user_id {
        #[derive(Debug, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Request {
            pub song_queue_id: uuid::Uuid,
            pub user_id: uuid::Uuid,
        }
    }

    pub mod get_songs {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Params {
            pub id: Option<uuid::Uuid>,
        }
    }
}

pub mod response {
    use serde::{Deserialize, Serialize};

    /// Song queue response
    #[derive(Default, Deserialize, Serialize, utoipa::ToSchema)]
    pub struct Response {
        pub message: String,
        /// Id of the queued song
        pub data: Vec<uuid::Uuid>,
    }

    pub mod fetch_queue_song {
        use serde::{Deserialize, Serialize};

        #[derive(Default, Deserialize, Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<crate::callers::song::song_queue::SongQueue>,
        }
    }

    pub mod update_status {
        #[derive(serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct ChangedStatus {
            pub old_status: String,
            pub new_status: String,
        }

        #[derive(Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<ChangedStatus>,
        }
    }

    pub mod update_song_queue {
        #[derive(Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<uuid::Uuid>,
        }
    }

    pub mod create_metadata {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<icarus_models::song::Song>,
        }
    }

    pub mod wipe_data_from_song_queue {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<uuid::Uuid>,
        }
    }

    pub mod link_user_id {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<uuid::Uuid>,
        }
    }

    pub mod get_songs {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<icarus_models::song::Song>,
        }
    }

    pub mod delete_song {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct SongAndCoverArt {
            pub song: icarus_models::song::Song,
            pub coverart: icarus_models::coverart::CoverArt,
        }

        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<SongAndCoverArt>,
        }
    }
}

// TODO: Might make a distinction between year and date in a song's tag at some point

pub mod status {
    pub const PENDING: &str = "pending";
    pub const READY: &str = "ready";
    pub const PROCESSING: &str = "processing";
    pub const DONE: &str = "done";

    pub async fn is_valid(status: &str) -> bool {
        status == PENDING || status == PROCESSING || status == DONE || status == READY
    }
}

mod song_queue {
    use sqlx::Row;

    // TODO: Move this somewhere else at some point
    #[derive(Debug, serde::Deserialize, serde::Serialize, sqlx::FromRow, utoipa::ToSchema)]
    pub struct SongQueue {
        pub id: uuid::Uuid,
        pub filename: String,
        pub status: String,
        pub user_id: uuid::Uuid,
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

    pub async fn update(
        pool: &sqlx::PgPool,
        data: &Vec<u8>,
        id: &uuid::Uuid,
    ) -> Result<Vec<u8>, sqlx::Error> {
        let result = sqlx::query(
            r#"
            UPDATE "songQueue" SET data = $1 WHERE id = $2 RETURNING data;
            "#,
        )
        .bind(data)
        .bind(id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error inserting: {e}");
        });

        match result {
            Ok(row) => Ok(row
                .try_get("data")
                .map_err(|_e| sqlx::Error::RowNotFound)
                .unwrap()),
            Err(_) => Err(sqlx::Error::RowNotFound),
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
            RETURNING id, filename, status, user_id;
            "#,
        )
        .bind(super::status::PROCESSING)
        .bind(super::status::READY)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error inserting: {e}");
        });

        match result {
            Ok(row) => {
                let user_id_result = row.try_get("user_id");
                let song_queue = SongQueue {
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
                    user_id: match user_id_result {
                        Ok(id) => id,
                        Err(_) => uuid::Uuid::nil(),
                    },
                };

                Ok(song_queue)
            }
            Err(_err) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn get_status_of_song_queue(
        pool: &sqlx::PgPool,
        id: &uuid::Uuid,
    ) -> Result<String, sqlx::Error> {
        let result = sqlx::query(
            r#"
            SELECT id, status FROM "songQueue" WHERE id = $1
            "#,
        )
        .bind(id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error selecting: {e}");
        });

        match result {
            Ok(row) => Ok(row
                .try_get("status")
                .map_err(|_e| sqlx::Error::RowNotFound)
                .unwrap()),
            Err(_err) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn update_song_queue_status(
        pool: &sqlx::PgPool,
        status: &String,
        id: &uuid::Uuid,
    ) -> Result<String, sqlx::Error> {
        let result = sqlx::query(
            r#"
            UPDATE "songQueue" SET status = $1 WHERE id = $2 RETURNING status;
            "#,
        )
        .bind(status)
        .bind(id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error updating record {e}");
        });

        match result {
            Ok(row) => Ok(row
                .try_get("status")
                .map_err(|_e| sqlx::Error::RowNotFound)
                .unwrap()),
            Err(_) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn link_user_id(
        pool: &sqlx::PgPool,
        id: &uuid::Uuid,
        user_id: &uuid::Uuid,
    ) -> Result<uuid::Uuid, sqlx::Error> {
        let result = sqlx::query(
            r#"
            UPDATE "songQueue" SET user_id = $1 WHERE id = $2 RETURNING user_id;
            "#,
        )
        .bind(user_id)
        .bind(id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error updating record {e}");
        });

        match result {
            Ok(row) => Ok(row
                .try_get("user_id")
                .map_err(|_e| sqlx::Error::RowNotFound)
                .unwrap()),
            Err(_) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn get_song_queue(
        pool: &sqlx::PgPool,
        id: &uuid::Uuid,
    ) -> Result<SongQueue, sqlx::Error> {
        let result = sqlx::query(
            r#"
            SELECT id, filename, status, user_id FROM "songQueue" WHERE id = $1
            "#,
        )
        .bind(id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error querying data: {e}");
        });

        match result {
            Ok(row) => {
                let user_id_result = row.try_get("user_id");
                let song_queue = SongQueue {
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
                    user_id: match user_id_result {
                        Ok(id) => id,
                        Err(_) => uuid::Uuid::nil(),
                    },
                };

                Ok(song_queue)
            }
            Err(_err) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn wipe_data(
        pool: &sqlx::PgPool,
        id: &uuid::Uuid,
    ) -> Result<uuid::Uuid, sqlx::Error> {
        let result = sqlx::query(
            r#"
            UPDATE "songQueue" SET data = NULL WHERE id = $1 RETURNING id;
            "#,
        )
        .bind(id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error updating record: {e}");
        });

        match result {
            Ok(row) => Ok(row
                .try_get("id")
                .map_err(|_e| sqlx::Error::RowNotFound)
                .unwrap()),
            Err(_) => Err(sqlx::Error::RowNotFound),
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
            eprintln!("Error inserting: {e}");
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

/// Module for song related endpoints
pub mod endpoint {
    use axum::{Json, http::StatusCode, response::IntoResponse};

    use crate::callers::song::song_queue;
    use crate::repo;

    /// Endpoint to queue a song. Starts the process and places the song in a queue
    #[utoipa::path(
        post,
        path = super::super::endpoints::QUEUESONG,
        request_body(
            content = super::request::song_queue::SongQueueRequest,
            description = "Multipart form data for uploading song",
            content_type = "multipart/form-data"
            ),
        responses(
            (status = 200, description = "Song queued", body = super::response::Response)
        )
    )]
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

            let raw_data: Vec<u8> = data.to_vec();
            let queue_repo = song_queue::insert(
                &pool,
                &raw_data,
                &file_name,
                &super::status::PENDING.to_string(),
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

    /// Endpoint to link a user id to a queued song
    #[utoipa::path(
        patch,
        path = super::super::endpoints::QUEUESONGLINKUSERID,
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
    pub async fn link_user_id(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::Json(payload): axum::Json<super::request::link_user_id::Request>,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::link_user_id::Response>,
    ) {
        let mut response = super::response::link_user_id::Response::default();

        match super::song_queue::get_song_queue(&pool, &payload.song_queue_id).await {
            Ok(song_queue) => {
                match super::song_queue::link_user_id(&pool, &song_queue.id, &payload.user_id).await
                {
                    Ok(user_id) => {
                        response.message = String::from(crate::callers::response::SUCCESSFUL);
                        response.data.push(user_id);
                        (axum::http::StatusCode::OK, axum::Json(response))
                    }
                    Err(err) => {
                        response.message = err.to_string();
                        (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                    }
                }
            }
            Err(err) => {
                response.message = err.to_string();
                (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
            }
        }
    }

    /// Endpoint to fetch the next queued song as long as it is available
    #[utoipa::path(
        get,
        path = super::super::endpoints::NEXTQUEUESONG,
        responses(
            (status = 200, description = "Queued song is present and available", body = super::response::fetch_queue_song::Response),
            (status = 400, description = "Linkage failed", body = super::response::fetch_queue_song::Response)
        )
    )]
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

    // TODO: Rename
    /// Endpoint to download the queued song
    #[utoipa::path(
        get,
        path = super::super::endpoints::QUEUESONGDATA,
        params(("id" = uuid::Uuid, Path, description = "Queued song Id")),
        responses(
            (status = 200, description = "Queued song linked", body = Vec<u8>),
            (status = 400, description = "Linkage failed", body = Vec<u8>)
        )
    )]
    pub async fn download_flac(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Path(id): axum::extract::Path<uuid::Uuid>,
    ) -> (StatusCode, axum::response::Response) {
        println!("Id: {id}");

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
                    format!("attachment; filename=\"{id}.flac\"")
                        .parse()
                        .unwrap(),
                );

                (StatusCode::OK, response)
            }
            Err(_err) => (StatusCode::BAD_REQUEST, axum::response::Response::default()),
        }
    }

    /// Endpoint to update the status of a queued song
    #[utoipa::path(
        patch,
        path = super::super::endpoints::QUEUESONG,
        request_body(
            content = super::request::update_status::Request,
            description = "Update the status of a queued song",
            content_type = "application/json"
            ),
        responses(
            (status = 200, description = "Status has been updated", body = super::response::update_status::Response),
            (status = 400, description = "Error updating status of queued song", body = super::response::update_status::Response)
        )
    )]
    pub async fn update_song_queue_status(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::Json(payload): axum::Json<super::request::update_status::Request>,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::update_status::Response>,
    ) {
        let mut response = super::response::update_status::Response::default();

        if super::status::is_valid(&payload.status).await {
            let id = payload.id;
            if !id.is_nil() {
                match super::song_queue::get_status_of_song_queue(&pool, &id).await {
                    Ok(old) => {
                        match super::song_queue::update_song_queue_status(
                            &pool,
                            &payload.status,
                            &id,
                        )
                        .await
                        {
                            Ok(new) => {
                                response.message = String::from("Successful");
                                response
                                    .data
                                    .push(super::response::update_status::ChangedStatus {
                                        old_status: old,
                                        new_status: new,
                                    });
                                (axum::http::StatusCode::OK, axum::Json(response))
                            }
                            Err(err) => {
                                response.message = err.to_string();
                                (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                            }
                        }
                    }
                    Err(err) => {
                        response.message = err.to_string();
                        (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                    }
                }
            } else {
                response.message = String::from("Id is nil");
                (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
            }
        } else {
            response.message = String::from("Status not valid");
            (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
        }
    }

    /// Endpoint to update the queued song data
    #[utoipa::path(
        patch,
        path = super::super::endpoints::QUEUESONGUPDATE,
        request_body(
            content = super::request::song_queue::SongQueueRequest,
            description = "Multipart form data for uploading song",
            content_type = "multipart/form-data"
            ),
        params(("id" = uuid::Uuid, Path, description = "Queued song Id")),
        responses(
            (status = 200, description = "Queued song updated", body = super::response::update_song_queue::Response),
            (status = 400, description = "Error updating queued song", body = super::response::update_song_queue::Response),
            (status = 404, description = "Queued song not found", body = super::response::update_song_queue::Response)
        )
    )]
    pub async fn update_song_queue(
        axum::extract::Path(id): axum::extract::Path<uuid::Uuid>,
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        mut multipart: axum::extract::Multipart,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::update_song_queue::Response>,
    ) {
        let mut response = super::response::update_song_queue::Response::default();

        if let Some(field) = multipart.next_field().await.unwrap() {
            let name = field.name().unwrap().to_string();
            let file_name = field.file_name().unwrap().to_string();
            let content_type = match field.content_type() {
                Some(ct) => ct.to_string(),
                None => String::new(),
            };

            let data = field.bytes().await.unwrap();

            println!(
                "Received file '{}' (name = '{}', content-type = '{}', size = {})",
                file_name,
                name,
                content_type,
                data.len()
            );

            let raw_data: Vec<u8> = data.to_vec();
            match song_queue::update(&pool, &raw_data, &id).await {
                Ok(_) => {
                    response.message = String::from("Successful");
                    response.data.push(id);
                    (axum::http::StatusCode::OK, axum::Json(response))
                }
                Err(err) => {
                    response.message = err.to_string();
                    (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                }
            }
        } else {
            response.message = String::from("No data provided");
            (axum::http::StatusCode::NOT_FOUND, axum::Json(response))
        }
    }

    /// Endpoint to create song
    #[utoipa::path(
        post,
        path = super::super::endpoints::QUEUEMETADATA,
        request_body(
            content = super::request::create_metadata::Request,
            description = "Data needed to create the song and save it to the filesystem",
            content_type = "application/json"
            ),
        responses(
            (status = 200, description = "Song created", body = super::response::create_metadata::Response),
            (status = 400, description = "Error", body = super::response::create_metadata::Response),
            (status = 505, description = "Error creating song", body = super::response::create_metadata::Response)
        )
    )]
    pub async fn create_metadata(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::Json(payload): axum::Json<super::request::create_metadata::Request>,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::create_metadata::Response>,
    ) {
        let mut response = super::response::create_metadata::Response::default();

        if payload.is_valid() {
            let mut song = payload.to_song();
            song.filename = icarus_models::song::generate_filename(
                icarus_models::types::MusicTypes::FlacExtension,
                true,
            );
            song.directory = icarus_envy::environment::get_root_directory().await.value;

            match song_queue::get_data(&pool, &payload.song_queue_id).await {
                Ok(data) => {
                    song.data = data;
                    let dir = std::path::Path::new(&song.directory);
                    if !dir.exists() {
                        println!("Creating directory");
                        match std::fs::create_dir_all(dir) {
                            Ok(_) => {
                                println!("Successfully created directory");
                            }
                            Err(err) => {
                                eprintln!("Error: Unable to create the directory {err:?}");
                            }
                        }
                    }

                    match song.save_to_filesystem() {
                        Ok(_) => match repo::song::insert(&pool, &song).await {
                            Ok((date_created, id)) => {
                                song.id = id;
                                song.date_created = Some(date_created);
                                response.message = String::from("Successful");
                                response.data.push(song);

                                (axum::http::StatusCode::OK, axum::Json(response))
                            }
                            Err(err) => {
                                response.message = format!("{:?} song {:?}", err.to_string(), song);
                                (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                            }
                        },
                        Err(err) => {
                            response.message = err.to_string();
                            (
                                axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                                axum::Json(response),
                            )
                        }
                    }
                }
                Err(err) => {
                    response.message = err.to_string();
                    (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                }
            }
        } else {
            response.message = String::from("Request body is not valid");
            (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
        }
    }

    /// Endpoint to wipe the data from a queued song
    #[utoipa::path(
        patch,
        path = super::super::endpoints::QUEUESONGDATAWIPE,
        request_body(
            content = super::request::wipe_data_from_song_queue::Request,
            description = "Pass the queued song Id to wipe the data",
            content_type = "application/json"
            ),
        responses(
            (status = 200, description = "Queued song data wiped", body = super::response::wipe_data_from_song_queue::Response),
            (status = 404, description = "Queued song cannot be found", body = super::response::wipe_data_from_song_queue::Response)
        )
    )]
    pub async fn wipe_data_from_song_queue(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::Json(payload): axum::Json<super::request::wipe_data_from_song_queue::Request>,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::wipe_data_from_song_queue::Response>,
    ) {
        let mut response = super::response::wipe_data_from_song_queue::Response::default();
        let id = payload.song_queue_id;

        match super::song_queue::get_song_queue(&pool, &id).await {
            Ok(song_queue) => match super::song_queue::wipe_data(&pool, &song_queue.id).await {
                Ok(wiped_id) => {
                    response.message = String::from("Success");
                    response.data.push(wiped_id);

                    (axum::http::StatusCode::OK, axum::Json(response))
                }
                Err(err) => {
                    response.message = err.to_string();
                    (axum::http::StatusCode::NOT_FOUND, axum::Json(response))
                }
            },
            Err(err) => {
                response.message = err.to_string();
                (axum::http::StatusCode::NOT_FOUND, axum::Json(response))
            }
        }
    }

    // Endpoint to get songs
    #[utoipa::path(
        get,
        path = super::super::endpoints::GETSONGS,
        params(
            ("id" = uuid::Uuid, Path, description = "Id of song")
            ),
        responses(
            (status = 200, description = "Songs found", body = super::response::get_songs::Response),
            (status = 400, description = "Error getting songs", body = super::response::get_songs::Response)
        )
    )]
    pub async fn get_songs(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Query(params): axum::extract::Query<super::request::get_songs::Params>,
    ) -> (
        axum::http::StatusCode,
        Json<super::response::get_songs::Response>,
    ) {
        let mut response = super::response::get_songs::Response::default();

        match params.id {
            Some(id) => match repo::song::get_song(&pool, &id).await {
                Ok(song) => {
                    response.message = String::from(super::super::response::SUCCESSFUL);
                    response.data.push(song);
                    (axum::http::StatusCode::OK, axum::Json(response))
                }
                Err(err) => {
                    response.message = err.to_string();
                    (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                }
            },
            None => {
                response.message = String::from("Invalid parameters");
                (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
            }
        }
    }

    /// Endpoint to get all songs
    #[utoipa::path(
        get,
        path = super::super::endpoints::GETALLSONGS,
        responses(
            (status = 200, description = "Getting all songs", body = super::response::get_songs::Response),
            (status = 404, description = "Song not found", body = super::response::get_songs::Response)
        )
    )]
    pub async fn get_all_songs(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::get_songs::Response>,
    ) {
        let mut response = super::response::get_songs::Response::default();

        match repo::song::get_all_songs(&pool).await {
            Ok(songs) => {
                response.message = String::from(super::super::response::SUCCESSFUL);
                response.data = songs;
                (axum::http::StatusCode::OK, axum::Json(response))
            }
            Err(err) => {
                response.message = err.to_string();
                (axum::http::StatusCode::NOT_FOUND, axum::Json(response))
            }
        }
    }

    /// Ednpoint to stream song
    #[utoipa::path(
        get,
        path = super::super::endpoints::STREAMSONG,
        params(("id" = uuid::Uuid, Path, description = "Song Id")),
        responses(
            (status = 200, description = "Stream song", body = Vec<u8>),
            (status = 500, description = "Error streaming song", body = (u64, String))
        )
    )]
    pub async fn stream_song(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Path(id): axum::extract::Path<uuid::Uuid>,
    ) -> impl IntoResponse {
        match repo::song::get_song(&pool, &id).await {
            Ok(song) => {
                let song_path = song.song_path().unwrap();
                let path = std::path::Path::new(&song_path);

                if !path.starts_with(&song.directory) || !path.exists() {
                    return Err((axum::http::StatusCode::NOT_FOUND, "File not found"));
                }

                let file = match tokio::fs::File::open(&path).await {
                    Ok(file) => file,
                    Err(_) => return Err((axum::http::StatusCode::NOT_FOUND, "File not found")),
                };

                let file_size = match file.metadata().await {
                    Ok(meta) => meta.len(),
                    Err(_) => {
                        return Err((
                            axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                            "Could not read file",
                        ));
                    }
                };

                let mime = mime_guess::from_path(path).first_or_octet_stream();
                let stream = tokio_util::io::ReaderStream::new(file);

                let rep = axum::response::Response::builder()
                    .header("content-type", mime.to_string())
                    .header("accept-ranges", "bytes")
                    .header("content-length", file_size.to_string())
                    .body(axum::body::Body::from_stream(stream))
                    .unwrap();

                Ok(rep)
            }
            Err(_err) => Err((
                axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                "Could not find file",
            )),
        }
    }

    /// Endpoint to download song
    #[utoipa::path(
        get,
        path = super::super::endpoints::DOWNLOADSONG,
        params(("id" = uuid::Uuid, Path, description = "Song Id")),
        responses(
            (status = 200, description = "Download song", body = (u64, Vec<u8>)),
            (status = 404, description = "Song not found", body = (u64, Vec<u8>)),
            (status = 400, description = "Error downloading song", body = (u64, Vec<u8>))
        )
    )]
    pub async fn download_song(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Path(id): axum::extract::Path<uuid::Uuid>,
    ) -> (axum::http::StatusCode, axum::response::Response) {
        match repo::song::get_song(&pool, &id).await {
            Ok(song) => match icarus_models::song::io::to_data(&song) {
                Ok(data) => {
                    let bytes = axum::body::Bytes::from(data);
                    let mut response = bytes.into_response();
                    let headers = response.headers_mut();
                    headers.insert(
                        axum::http::header::CONTENT_TYPE,
                        "audio/flac".parse().unwrap(),
                    );
                    headers.insert(
                        axum::http::header::CONTENT_DISPOSITION,
                        format!("attachment; filename=\"{id}.flac\"")
                            .parse()
                            .unwrap(),
                    );

                    (axum::http::StatusCode::OK, response)
                }
                Err(_err) => (
                    axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                    axum::response::Response::default(),
                ),
            },
            Err(_err) => (
                axum::http::StatusCode::NOT_FOUND,
                axum::response::Response::default(),
            ),
        }
    }

    /// Endpoint to delete the song
    #[utoipa::path(
        delete,
        path = super::super::endpoints::DELETESONG,
        params(("id" = uuid::Uuid, Path, description = "Song Id")),
        responses(
            (status = 200, description = "Song deleted", body = super::response::delete_song::Response),
            (status = 404, description = "Song not found", body = super::response::delete_song::Response),
            (status = 500, description = "Error deleting song", body = super::response::delete_song::Response)
        )
    )]
    pub async fn delete_song(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Path(id): axum::extract::Path<uuid::Uuid>,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::delete_song::Response>,
    ) {
        let mut response = super::response::delete_song::Response::default();

        match repo::song::get_song(&pool, &id).await {
            Ok(song) => {
                match super::super::coverart::cov_db::get_coverart_with_song_id(&pool, &song.id)
                    .await
                {
                    Ok(coverart) => {
                        let coverart_path_str = match coverart.get_path() {
                            Ok(path) => path,
                            Err(err) => {
                                response.message = err.to_string();
                                return (
                                    axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                                    axum::Json(response),
                                );
                            }
                        };
                        let coverart_path = std::path::Path::new(&coverart_path_str);

                        if coverart_path.exists() {
                            match repo::song::delete_song(&pool, &song.id).await {
                                Ok(deleted_song) => {
                                    match super::super::coverart::cov_db::delete_coverart(
                                        &pool,
                                        &coverart.id,
                                    )
                                    .await
                                    {
                                        Ok(deleted_coverart) => {
                                            match song.remove_from_filesystem() {
                                                Ok(_) => match coverart.remove_from_filesystem() {
                                                    Ok(_) => {
                                                        response.message = String::from(
                                                            super::super::response::SUCCESSFUL,
                                                        );
                                                        response.data.push(super::response::delete_song::SongAndCoverArt{ song: deleted_song, coverart: deleted_coverart });
                                                        (
                                                            axum::http::StatusCode::OK,
                                                            axum::Json(response),
                                                        )
                                                    }
                                                    Err(err) => {
                                                        response.message = err.to_string();
                                                        (axum::http::StatusCode::INTERNAL_SERVER_ERROR, axum::Json(response))
                                                    }
                                                },
                                                Err(err) => {
                                                    response.message = err.to_string();
                                                    (axum::http::StatusCode::INTERNAL_SERVER_ERROR, axum::Json(response))
                                                }
                                            }
                                        }

                                        Err(err) => {
                                            response.message = err.to_string();
                                            (
                                                axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                                                axum::Json(response),
                                            )
                                        }
                                    }
                                }
                                Err(err) => {
                                    response.message = err.to_string();
                                    (
                                        axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                                        axum::Json(response),
                                    )
                                }
                            }
                        } else {
                            response.message =
                                String::from("Could not locate coverart on the filesystem");
                            (
                                axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                                axum::Json(response),
                            )
                        }
                    }
                    Err(err) => {
                        response.message = err.to_string();
                        (axum::http::StatusCode::NOT_FOUND, axum::Json(response))
                    }
                }
            }
            Err(err) => {
                response.message = err.to_string();
                (axum::http::StatusCode::NOT_FOUND, axum::Json(response))
            }
        }
    }
}
