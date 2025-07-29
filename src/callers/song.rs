// TODO: Separate queue and song endpoints
pub mod request {
    use serde::{Deserialize, Serialize};

    #[derive(Default, Deserialize, Serialize)]
    pub struct Request {
        pub message: String,
    }

    pub mod update_status {
        #[derive(Default, serde::Deserialize, serde::Serialize)]
        pub struct Request {
            pub id: uuid::Uuid,
            pub status: String,
        }
    }

    pub mod create_metadata {
        #[derive(Debug, serde::Deserialize, serde::Serialize)]
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
                    // TODO: Change the type of this in icarus_models lib
                    date_created: String::new(),
                    filename: String::new(),
                    data: Vec::new(),
                    directory: String::new(),
                }
            }
        }
    }

    pub mod wipe_data_from_song_queue {
        #[derive(Debug, serde::Deserialize, serde::Serialize)]
        pub struct Request {
            pub song_queue_id: uuid::Uuid,
        }
    }

    pub mod link_user_id {
        #[derive(Debug, serde::Deserialize, serde::Serialize)]
        pub struct Request {
            pub song_queue_id: uuid::Uuid,
            pub user_id: uuid::Uuid,
        }
    }

    pub mod get_songs {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize)]
        pub struct Params {
            pub id: Option<uuid::Uuid>,
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

    pub mod fetch_queue_song {
        use serde::{Deserialize, Serialize};

        #[derive(Default, Deserialize, Serialize)]
        pub struct Response {
            pub message: String,
            pub data: Vec<crate::callers::song::song_queue::SongQueue>,
        }
    }

    pub mod update_status {
        #[derive(serde::Deserialize, serde::Serialize)]
        pub struct ChangedStatus {
            pub old_status: String,
            pub new_status: String,
        }

        #[derive(Default, serde::Deserialize, serde::Serialize)]
        pub struct Response {
            pub message: String,
            pub data: Vec<ChangedStatus>,
        }
    }

    pub mod update_song_queue {
        #[derive(Default, serde::Deserialize, serde::Serialize)]
        pub struct Response {
            pub message: String,
            pub data: Vec<uuid::Uuid>,
        }
    }

    pub mod create_metadata {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize)]
        pub struct Response {
            pub message: String,
            pub data: Vec<icarus_models::song::Song>,
        }
    }

    pub mod wipe_data_from_song_queue {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize)]
        pub struct Response {
            pub message: String,
            pub data: Vec<uuid::Uuid>,
        }
    }

    pub mod link_user_id {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize)]
        pub struct Response {
            pub message: String,
            pub data: Vec<uuid::Uuid>,
        }
    }

    pub mod get_songs {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize)]
        pub struct Response {
            pub message: String,
            pub data: Vec<icarus_models::song::Song>,
        }
    }

    pub mod delete_song {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize)]
        pub struct Response {
            pub message: String,
            pub data: Vec<(icarus_models::song::Song, icarus_models::coverart::CoverArt)>
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

pub mod song_db {
    use sqlx::Row;

    // TODO: Change first parameter of return value from string to a time type
    pub async fn insert(
        pool: &sqlx::PgPool,
        song: &icarus_models::song::Song,
    ) -> Result<(String, uuid::Uuid), sqlx::Error> {
        let result = sqlx::query(
            r#"
            INSERT INTO "song" (title, artist, album_artist, album, genre, year, track, disc, track_count, disc_count, duration, audio_type, filename, directory, user_id) 
            VALUES($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15) RETURNING date_created, id;
            "#
            )
            .bind(&song.title)
            .bind(&song.artist)
            .bind(&song.album_artist)
            .bind(&song.album)
            .bind(&song.genre)
            .bind(song.year)
            .bind(song.track)
            .bind(song.disc)
            .bind(song.track_count)
            .bind(song.disc_count)
            .bind(song.duration)
            .bind(&song.audio_type)
            .bind(&song.filename)
            .bind(&song.directory)
            .bind(song.user_id)
            .fetch_one(pool)
            .await
            .map_err(|e| {
                eprintln!("Error inserting query: {e}");
            });

        match result {
            Ok(row) => {
                let id: uuid::Uuid = row
                    .try_get("id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap();
                let date_created_time: time::OffsetDateTime = row
                    .try_get("date_created")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap();
                let date_created = date_created_time.to_string();

                Ok((date_created, id))
            }
            Err(_) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn get_song(
        pool: &sqlx::PgPool,
        id: &uuid::Uuid,
    ) -> Result<icarus_models::song::Song, sqlx::Error> {
        let result = sqlx::query(
            r#"
            SELECT * FROM "song" WHERE id = $1
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
                let date_created_time: time::OffsetDateTime = row
                    .try_get("date_created")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap();

                Ok(icarus_models::song::Song {
                    id: row
                        .try_get("id")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    title: row
                        .try_get("title")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    artist: row
                        .try_get("artist")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    album_artist: row
                        .try_get("album_artist")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    album: row
                        .try_get("album")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    genre: row
                        .try_get("genre")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    year: row
                        .try_get("year")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    track: row
                        .try_get("track")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    disc: row
                        .try_get("disc")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    track_count: row
                        .try_get("track_count")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    disc_count: row
                        .try_get("disc_count")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    duration: row
                        .try_get("duration")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    audio_type: row
                        .try_get("audio_type")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    filename: row
                        .try_get("filename")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    directory: row
                        .try_get("directory")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    date_created: date_created_time.to_string(),
                    user_id: row
                        .try_get("user_id")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    data: Vec::new(),
                })
            }
            Err(_) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn delete_song(pool: &sqlx::PgPool, id: &uuid::Uuid) -> Result<icarus_models::song::Song, sqlx::Error> {
        let result = sqlx::query(
            // icarus_models::song::Song,
            r#"
            DELETE FROM "song"
            WHERE id = $1
            RETURNING id, title, artist, album_artist, genre, year, disc, track_count, disc_count, duration, audio_type, date_created, filename, directory, user_id
            "#,
        )
        .bind(id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error deleting data: {e:?}")
        });

        match result {
            Ok(row) => {
                let date_created_time: time::OffsetDateTime = row
                    .try_get("date_created")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap();

                Ok(icarus_models::song::Song {
                    id: row
                        .try_get("id")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    title: row
                        .try_get("title")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    artist: row
                        .try_get("artist")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    album_artist: row
                        .try_get("album_artist")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    album: row
                        .try_get("album")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    genre: row
                        .try_get("genre")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    year: row
                        .try_get("year")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    track: row
                        .try_get("track")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    disc: row
                        .try_get("disc")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    track_count: row
                        .try_get("track_count")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    disc_count: row
                        .try_get("disc_count")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    duration: row
                        .try_get("duration")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    audio_type: row
                        .try_get("audio_type")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    filename: row
                        .try_get("filename")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    directory: row
                        .try_get("directory")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    date_created: date_created_time.to_string(),
                    user_id: row
                        .try_get("user_id")
                        .map_err(|_e| sqlx::Error::RowNotFound)
                        .unwrap(),
                    data: Vec::new(),
                })
            }
            Err(_) => Err(sqlx::Error::RowNotFound),
        }
    }
}

mod song_queue {
    use sqlx::Row;

    #[derive(Debug, serde::Serialize, sqlx::FromRow)]
    pub struct InsertedData {
        pub id: uuid::Uuid,
    }

    #[derive(Debug, serde::Deserialize, serde::Serialize, sqlx::FromRow)]
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

            // TODO: Remove this
            // Save the file to disk
            let mut file = std::fs::File::create(&file_name).unwrap();
            file.write_all(&data).unwrap();

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
            song.filename =
                song.generate_filename(icarus_models::types::MusicTypes::FlacExtension, true);
            song.directory = icarus_envy::environment::get_root_directory().await;

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

                    let save_path = dir.join(&song.filename);

                    match std::fs::File::create(&save_path) {
                        Ok(mut file) => {
                            file.write_all(&song.data).unwrap();

                            match song.song_path() {
                                Ok(_) => match super::song_db::insert(&pool, &song).await {
                                    Ok((date_created, id)) => {
                                        song.id = id;
                                        song.date_created = date_created;
                                        response.message = String::from("Successful");
                                        response.data.push(song);

                                        (axum::http::StatusCode::OK, axum::Json(response))
                                    }
                                    Err(err) => {
                                        response.message =
                                            format!("{:?} song {:?}", err.to_string(), song);
                                        (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                                    }
                                },
                                Err(err) => {
                                    response.message = err.to_string();
                                    (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                                }
                            }
                        }
                        Err(err) => {
                            let song_path = song.song_path();
                            response.message = format!(
                                "{err:?} Song directory: {} Filename: {} Save Path: {:?} Song Path: {:?}",
                                song.directory, song.filename, save_path, song_path
                            );
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

    pub async fn get_songs(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Query(params): axum::extract::Query<super::request::get_songs::Params>,
    ) -> (
        axum::http::StatusCode,
        Json<super::response::get_songs::Response>,
    ) {
        let mut response = super::response::get_songs::Response::default();

        match params.id {
            Some(id) => match super::song_db::get_song(&pool, &id).await {
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

    pub async fn stream_song(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Path(id): axum::extract::Path<uuid::Uuid>,
    ) -> impl IntoResponse {
        match super::song_db::get_song(&pool, &id).await {
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

    pub async fn download_song(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Path(id): axum::extract::Path<uuid::Uuid>,
    ) -> (axum::http::StatusCode, axum::response::Response) {
        match super::song_db::get_song(&pool, &id).await {
            Ok(song) => match song.to_data() {
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

    pub async fn delete_song(axum::Extension(pool): axum::Extension<sqlx::PgPool>, axum::extract::Path(id): axum::extract::Path<uuid::Uuid>)
    -> (axum::http::StatusCode, axum::Json<super::response::delete_song::Response>) {
        let mut response = super::response::delete_song::Response::default();

        match super::song_db::get_song(&pool, &id).await {
            Ok(song) => match super::super::coverart::cov_db::get_coverart_with_song_id(&pool, &song.id).await {
                Ok(coverart) => {
                    let coverart_path = std::path::Path::new(&coverart.path);
                    if coverart_path.exists() {
                        match song.song_path() {
                            Ok(song_path) => match super::song_db::delete_song(&pool, &song.id).await {
                                Ok(deleted_song) => match super::super::coverart::cov_db::delete_coverart(&pool, &coverart.id).await {
                                    Ok(deleted_coverart) => match std::fs::remove_file(song_path) {
                                        Ok(_) => match std::fs::remove_file(&coverart.path) {
                                            Ok(_) => {
                                                response.data.push((deleted_song, deleted_coverart));
                                                (axum::http::StatusCode::INTERNAL_SERVER_ERROR, axum::Json(response))
                                            }
                                            Err(err) => {
                                                response.message = String::from("Could not locate coverart on the filesystem");
                                                (axum::http::StatusCode::INTERNAL_SERVER_ERROR, axum::Json(response))
                                            }
                                        }
                                        Err(err) => {
                                            response.message = String::from("Could not locate coverart on the filesystem");
                                            (axum::http::StatusCode::INTERNAL_SERVER_ERROR, axum::Json(response))
                                        }
                                    }

                                    Err(err) => {
                                        response.message = String::from("Could not locate coverart on the filesystem");
                                        (axum::http::StatusCode::INTERNAL_SERVER_ERROR, axum::Json(response))
                                    }
                                }
                                Err(err) => {
                                        response.message = String::from("Could not locate coverart on the filesystem");
                                        (axum::http::StatusCode::INTERNAL_SERVER_ERROR, axum::Json(response))
                                }
                            }
                            Err(err) => {
                                response.message = String::from("Could not locate coverart on the filesystem");
                                (axum::http::StatusCode::INTERNAL_SERVER_ERROR, axum::Json(response))
                            }
                        }
                    } else {
                        response.message = String::from("Could not locate coverart on the filesystem");
                        (axum::http::StatusCode::INTERNAL_SERVER_ERROR, axum::Json(response))
                    }
                }
                Err(err) => {
                    response.message = err.to_string();
                    (axum::http::StatusCode::NOT_FOUND, axum::Json(response))
                }
            }
            Err(err) => {
                response.message = err.to_string();
                (axum::http::StatusCode::NOT_FOUND, axum::Json(response))
            }
        }
    }
}
