// TODO: Separate queue and coverart endpoints
#[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
pub struct CoverArtQueue {
    pub id: uuid::Uuid,
    pub file_type: String,
    pub song_queue_id: uuid::Uuid,
}

pub mod request {
    pub mod queue {
        #[derive(utoipa::ToSchema)]
        pub struct Request {
            /// Filename
            pub file: String,
            #[schema(rename = "type")]
            /// File type. Should be a file and not a value
            pub file_type: String,
            /// Raw data of the cover art file
            pub value: Vec<u8>,
        }
    }

    pub mod link {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Request {
            pub coverart_id: uuid::Uuid,
            pub song_queue_id: uuid::Uuid,
        }
    }

    pub mod fetch_coverart_no_data {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Params {
            pub id: Option<uuid::Uuid>,
            pub song_queue_id: Option<uuid::Uuid>,
        }
    }

    pub mod fetch_coverart_with_data {
        #[derive(Debug, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Params {
            pub id: Option<uuid::Uuid>,
            pub song_queue_id: Option<uuid::Uuid>,
        }
    }

    pub mod create_coverart {
        #[derive(Debug, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Request {
            pub song_id: uuid::Uuid,
            pub coverart_queue_id: uuid::Uuid,
        }
    }

    pub mod wipe_data_from_coverart_queue {
        #[derive(Debug, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Request {
            pub coverart_queue_id: uuid::Uuid,
        }
    }

    pub mod get_coverart {
        #[derive(Debug, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Params {
            pub id: Option<uuid::Uuid>,
        }
    }
}

pub mod response {
    #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
    pub struct Response {
        pub message: String,
        pub data: Vec<uuid::Uuid>,
    }

    pub mod link {
        #[derive(Debug, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Id {
            pub coverart_id: uuid::Uuid,
            pub song_queue_id: uuid::Uuid,
        }

        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<Id>,
        }
    }

    pub mod fetch_coverart_no_data {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<super::super::CoverArtQueue>,
        }
    }

    pub mod fetch_coverart_with_data {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<Vec<u8>>,
        }
    }

    pub mod create_coverart {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<icarus_models::coverart::CoverArt>,
        }
    }

    pub mod wipe_data_from_coverart_queue {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<uuid::Uuid>,
        }
    }

    pub mod get_coverart {
        #[derive(Debug, Default, serde::Deserialize, serde::Serialize, utoipa::ToSchema)]
        pub struct Response {
            pub message: String,
            pub data: Vec<icarus_models::coverart::CoverArt>,
        }
    }
}

pub mod db {
    use sqlx::Row;

    pub async fn insert(
        pool: &sqlx::PgPool,
        data: &Vec<u8>,
        file_type: &str,
    ) -> Result<uuid::Uuid, sqlx::Error> {
        let result = sqlx::query(
            r#"
            INSERT INTO "coverartQueue" (data, file_type) VALUES($1, $2) RETURNING id;
            "#,
        )
        .bind(data)
        .bind(file_type)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error inserting: {e:?}");
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

    pub async fn get_coverart_queue_with_id(
        pool: &sqlx::PgPool,
        id: &uuid::Uuid,
    ) -> Result<super::CoverArtQueue, sqlx::Error> {
        let result = sqlx::query(
            r#"
            SELECT id, file_type, song_queue_id FROM "coverartQueue" WHERE id = $1;
            "#,
        )
        .bind(id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error querying data: {e:?}");
        });

        match result {
            Ok(row) => Ok(super::CoverArtQueue {
                id: row
                    .try_get("id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                file_type: row
                    .try_get("file_type")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                song_queue_id: row
                    .try_get("song_queue_id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
            }),
            Err(_) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn get_coverart_queue_with_song_queue_id(
        pool: &sqlx::PgPool,
        song_queue_id: &uuid::Uuid,
    ) -> Result<super::CoverArtQueue, sqlx::Error> {
        let result = sqlx::query(
            r#"
            SELECT id, file_type, song_queue_id FROM "coverartQueue" WHERE song_queue_id = $1;
            "#,
        )
        .bind(song_queue_id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error querying data: {e:?}");
        });

        match result {
            Ok(row) => Ok(super::CoverArtQueue {
                id: row
                    .try_get("id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                file_type: row
                    .try_get("file_type")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                song_queue_id: row
                    .try_get("song_queue_id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
            }),
            Err(_) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn get_coverart_queue_data_with_id(
        pool: &sqlx::PgPool,
        id: &uuid::Uuid,
    ) -> Result<Vec<u8>, sqlx::Error> {
        let result = sqlx::query(
            r#"
            SELECT data FROM "coverartQueue" WHERE id = $1;
            "#,
        )
        .bind(id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error querying data: {e:?}");
        });

        match result {
            Ok(row) => Ok(row
                .try_get("data")
                .map_err(|_e| sqlx::Error::RowNotFound)
                .unwrap()),
            Err(_) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn get_coverart_queue_data_with_song_queue_id(
        pool: &sqlx::PgPool,
        song_queue_id: &uuid::Uuid,
    ) -> Result<Vec<u8>, sqlx::Error> {
        let result = sqlx::query(
            r#"
            SELECT data FROM "coverartQueue" WHERE song_queue_id = $1;
            "#,
        )
        .bind(song_queue_id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error querying data: {e}");
        });

        match result {
            Ok(row) => Ok(row
                .try_get("data")
                .map_err(|_e| sqlx::Error::RowNotFound)
                .unwrap()),
            Err(_) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn wipe_data(
        pool: &sqlx::PgPool,
        coverart_queue_id: &uuid::Uuid,
    ) -> Result<uuid::Uuid, sqlx::Error> {
        let result = sqlx::query(
            r#"
            UPDATE "coverartQueue" SET data = NULL WHERE id = $1 RETURNING id;
            "#,
        )
        .bind(coverart_queue_id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error updating query: {e}");
        });

        match result {
            Ok(row) => Ok(row
                .try_get("id")
                .map_err(|_e| sqlx::Error::RowNotFound)
                .unwrap()),
            Err(_) => Err(sqlx::Error::RowNotFound),
        }
    }
}

pub mod cov_db {
    use sqlx::Row;

    pub async fn create(
        pool: &sqlx::PgPool,
        coverart: &icarus_models::coverart::CoverArt,
        song_id: &uuid::Uuid,
    ) -> Result<uuid::Uuid, sqlx::Error> {
        let result = sqlx::query(
            r#"
            INSERT INTO "coverart" (title, directory, filename, song_id) VALUES($1, $2, $3, $4) RETURNING id;
            "#,
        )
        .bind(&coverart.title)
        .bind(&coverart.directory)
        .bind(&coverart.filename)
        .bind(song_id)
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

    pub async fn get_coverart(
        pool: &sqlx::PgPool,
        id: &uuid::Uuid,
    ) -> Result<icarus_models::coverart::CoverArt, sqlx::Error> {
        let result = sqlx::query(
            r#"
            SELECT id, title, directory, filename, song_id FROM "coverart" WHERE id = $1;
            "#,
        )
        .bind(id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error querying data: {e:?}");
        });

        match result {
            Ok(row) => Ok(icarus_models::coverart::CoverArt {
                id: row
                    .try_get("id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                title: row
                    .try_get("title")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                directory: row
                    .try_get("directory")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                filename: row
                    .try_get("filename")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                song_id: row
                    .try_get("song_id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                ..Default::default()
            }),
            Err(_) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn get_coverart_with_song_id(
        pool: &sqlx::PgPool,
        song_id: &uuid::Uuid,
    ) -> Result<icarus_models::coverart::CoverArt, sqlx::Error> {
        let result = sqlx::query(
            r#"
            SELECT id, title, directory, filename, song_id FROM "coverart" WHERE song_id = $1;
            "#,
        )
        .bind(song_id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error querying data: {e:?}");
        });

        match result {
            Ok(row) => Ok(icarus_models::coverart::CoverArt {
                id: row
                    .try_get("id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                title: row
                    .try_get("title")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                directory: row
                    .try_get("directory")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                filename: row
                    .try_get("filename")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                data: Vec::new(),
                song_id: row
                    .try_get("song_id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
            }),
            Err(_) => Err(sqlx::Error::RowNotFound),
        }
    }

    pub async fn delete_coverart(
        pool: &sqlx::PgPool,
        id: &uuid::Uuid,
    ) -> Result<icarus_models::coverart::CoverArt, sqlx::Error> {
        let result = sqlx::query(
            r#"
            DELETE FROM "coverart"
            WHERE id = $1
            RETURNING id, title, directory, filename, song_id
            "#,
        )
        .bind(id)
        .fetch_one(pool)
        .await
        .map_err(|e| {
            eprintln!("Error deleting data: {e:?}");
        });

        match result {
            Ok(row) => Ok(icarus_models::coverart::CoverArt {
                id: row
                    .try_get("id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                title: row
                    .try_get("title")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                directory: row
                    .try_get("directory")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                filename: row
                    .try_get("filename")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                song_id: row
                    .try_get("song_id")
                    .map_err(|_e| sqlx::Error::RowNotFound)
                    .unwrap(),
                data: Vec::new(),
            }),
            Err(_err) => Err(sqlx::Error::RowNotFound),
        }
    }
}

mod helper {
    pub fn is_coverart_file_type_valid(file_type: &String) -> bool {
        let valid_file_types = vec![
            String::from(icarus_meta::detection::coverart::constants::JPEG_TYPE),
            String::from(icarus_meta::detection::coverart::constants::JPG_TYPE),
            String::from(icarus_meta::detection::coverart::constants::PNG_TYPE),
        ];

        for valid_file_type in valid_file_types {
            if valid_file_type == *file_type {
                return true;
            }
        }

        false
    }
}

pub mod endpoint {
    use axum::response::IntoResponse;

    /// Endpoint to queue cover art
    #[utoipa::path(
        post,
        path = super::super::endpoints::QUEUECOVERART,
        request_body(
            content = super::request::queue::Request,
            ),
        responses(
            (status = 200, description = "Successful", body = super::response::Response),
            (status = 400, description = "Error queueing cover art", body = super::response::Response)
        )
    )]
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
                let file_type =
                    match icarus_meta::detection::coverart::file_type_from_data(&raw_data) {
                        Ok(file_type) => file_type,
                        Err(err) => {
                            eprintln!("Error: {err:?}");
                            response.message = err.to_string();
                            return (
                                axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                                axum::Json(response),
                            );
                        }
                    };

                if !super::helper::is_coverart_file_type_valid(&file_type.file_type) {
                    response.message = format!("CoverArt file type not supported: {file_type:?}");
                    (
                        axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                        axum::Json(response),
                    )
                } else {
                    println!(
                        "Received file '{}' (name = '{}', content-type = '{}', size = {}, file-type = {:?})",
                        file_name,
                        name,
                        content_type,
                        data.len(),
                        file_type
                    );

                    match super::db::insert(&pool, &raw_data, &file_type.file_type).await {
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
            }
            Ok(None) => (axum::http::StatusCode::BAD_REQUEST, axum::Json(response)),
            Err(err) => {
                response.message = err.to_string();
                (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
            }
        }
    }

    /// Endpoint to link queued cover art
    #[utoipa::path(
        post,
        path = super::super::endpoints::QUEUECOVERARTLINK,
        request_body(
            content = super::request::link::Request,
            description = "Linking queued cover art to queued song",
            content_type = "application/json"
            ),
        responses(
            (status = 200, description = "Queued cover art linked", body = super::response::link::Response),
            (status = 400, description = "Linkage failed", body = super::response::link::Response)
        )
    )]
    pub async fn link(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::Json(payload): axum::Json<super::request::link::Request>,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::link::Response>,
    ) {
        let mut response = super::response::link::Response::default();
        let id = payload.coverart_id;
        let song_id = payload.song_queue_id;

        match super::db::update(&pool, &id, &song_id).await {
            Ok(_o) => {
                response.data.push(super::response::link::Id {
                    song_queue_id: song_id,
                    coverart_id: id,
                });

                (axum::http::StatusCode::OK, axum::Json(response))
            }
            Err(err) => {
                response.message = err.to_string();
                (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
            }
        }
    }

    /// Endpoint to fetch cover art details
    #[utoipa::path(
        get,
        path = super::super::endpoints::QUEUECOVERART,
        params(
            ("id" = uuid::Uuid, Path, description = "Queued cover art Id"),
            ("song_queue_id" = uuid::Uuid, Path, description = "Queued song Id")
        ),
        responses(
            (status = 200, description = "Queued song linked", body = super::response::fetch_coverart_no_data::Response),
            (status = 400, description = "Linkage failed", body = super::response::fetch_coverart_no_data::Response)
        )
    )]
    pub async fn fetch_coverart_no_data(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Query(params): axum::extract::Query<
            super::request::fetch_coverart_no_data::Params,
        >,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::fetch_coverart_no_data::Response>,
    ) {
        let mut response = super::response::fetch_coverart_no_data::Response::default();

        match params.id {
            Some(id) => match super::db::get_coverart_queue_with_id(&pool, &id).await {
                Ok(cover_art_queue) => {
                    response.message = String::from("Successful");
                    response.data.push(cover_art_queue);
                    (axum::http::StatusCode::OK, axum::Json(response))
                }
                Err(err) => {
                    response.message = err.to_string();
                    (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                }
            },
            _ => match params.song_queue_id {
                Some(song_queue_id) => {
                    match super::db::get_coverart_queue_with_song_queue_id(&pool, &song_queue_id)
                        .await
                    {
                        Ok(cover_art_queue) => {
                            response.message = String::from("Successful");
                            response.data.push(cover_art_queue);
                            (axum::http::StatusCode::OK, axum::Json(response))
                        }
                        Err(err) => {
                            response.message = err.to_string();
                            (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                        }
                    }
                }
                None => {
                    response.message = String::from("No valid id provided");
                    (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                }
            },
        }
    }

    /// Endpoint to fetch the queued cover art data
    #[utoipa::path(
        get,
        path = super::super::endpoints::QUEUECOVERARTDATA,
        params(("id" = uuid::Uuid, Path, description = "Queued cover art Id")),
        responses(
            (status = 200, description = "Queued cover art data", body = Vec<u8>),
            (status = 400, description = "Error fetching queued cover art data", body = Vec<u8>)
        )
    )]
    pub async fn fetch_coverart_with_data(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Path(id): axum::extract::Path<uuid::Uuid>,
    ) -> (axum::http::StatusCode, axum::response::Response) {
        match super::db::get_coverart_queue_data_with_id(&pool, &id).await {
            Ok(data) => {
                let bytes = axum::body::Bytes::from(data);
                let mut response = bytes.into_response();
                let headers = response.headers_mut();
                // TODO: Address this hard coding for the coverart content type
                headers.insert(axum::http::header::CONTENT_TYPE, "image".parse().unwrap());
                // TODO: Make the conent disposition more dynamic
                headers.insert(
                    axum::http::header::CONTENT_DISPOSITION,
                    format!("attachment; filename=\"{id}.jpg\"")
                        .parse()
                        .unwrap(),
                );

                (axum::http::StatusCode::OK, response)
            }
            Err(_err) => (
                axum::http::StatusCode::BAD_REQUEST,
                axum::response::Response::default(),
            ),
        }
    }

    /// Endpoint to create cover art
    #[utoipa::path(
        post,
        path = super::super::endpoints::CREATECOVERART,
        request_body(
            content = super::request::create_coverart::Request,
            description = "Data required to create cover art",
            content_type = "application/json"
            ),
        responses(
            (status = 200, description = "Cover art created", body = super::response::create_coverart::Response),
            (status = 400, description = "Failure in creating cover art", body = super::response::create_coverart::Response)
        )
    )]
    pub async fn create_coverart(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::Json(payload): axum::Json<super::request::create_coverart::Request>,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::create_coverart::Response>,
    ) {
        let mut response = super::response::create_coverart::Response::default();
        let id = payload.coverart_queue_id;

        match super::db::get_coverart_queue_data_with_id(&pool, &id).await {
            Ok(data) => {
                let song_id = payload.song_id;
                match crate::repo::song::get_song(&pool, &song_id).await {
                    Ok(song) => {
                        let directory = icarus_envy::environment::get_root_directory().await.value;
                        // TODO: Make this random and the file extension should not be hard coded
                        let filename = format!("{}-coverart.jpeg", &song.filename[..8]);

                        let mut coverart =
                            icarus_models::coverart::init::init_coverart_dir_and_filename(
                                &directory, &filename,
                            );
                        coverart.title = song.album.clone();
                        coverart.data = data;

                        match coverart.save_to_filesystem() {
                            Ok(_) => {
                                match super::cov_db::create(&pool, &coverart, &song.id).await {
                                    Ok(id) => {
                                        coverart.song_id = song_id;
                                        coverart.id = id;
                                        println!("Cover Art created");

                                        response.message = String::from("Successful");
                                        response.data.push(coverart);

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
            }
            Err(err) => {
                response.message = err.to_string();
                (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
            }
        }
    }

    /// Endpoint to wipe data from the cover art queue
    #[utoipa::path(
        patch,
        path = super::super::endpoints::QUEUECOVERARTDATAWIPE,
        request_body(
            content = super::request::wipe_data_from_coverart_queue::Request,
            description = "Data required to wipe the data from the cover art queue",
            content_type = "application/json"
            ),
        responses(
            (status = 200, description = "Data wiped from cover art queue", body = super::response::wipe_data_from_coverart_queue::Response),
            (status = 400, description = "Error wiping the data", body = super::response::wipe_data_from_coverart_queue::Response),
            (status = 404, description = "Cover art not found", body = super::response::wipe_data_from_coverart_queue::Response)
        )
    )]
    pub async fn wipe_data_from_coverart_queue(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::Json(payload): axum::Json<super::request::wipe_data_from_coverart_queue::Request>,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::wipe_data_from_coverart_queue::Response>,
    ) {
        let mut response = super::response::wipe_data_from_coverart_queue::Response::default();
        let coverart_queue_id = payload.coverart_queue_id;

        match super::db::get_coverart_queue_with_id(&pool, &coverart_queue_id).await {
            Ok(coverart_queue) => match super::db::wipe_data(&pool, &coverart_queue.id).await {
                Ok(id) => {
                    response.message = String::from("Success");
                    response.data.push(id);
                    (axum::http::StatusCode::OK, axum::Json(response))
                }
                Err(err) => {
                    response.message = err.to_string();
                    (axum::http::StatusCode::BAD_REQUEST, axum::Json(response))
                }
            },
            Err(err) => {
                response.message = err.to_string();
                (axum::http::StatusCode::NOT_FOUND, axum::Json(response))
            }
        }
    }

    /// Endpoint to get cover art with criteria
    #[utoipa::path(
        get,
        path = super::super::endpoints::GETCOVERART,
        params(
            ("id" = uuid::Uuid, Path, description = "Cover art Id")
            ),
        responses(
            (status = 200, description = "Cover art retrieved", body = super::response::get_coverart::Response),
            (status = 400, description = "Error retrieving cover art", body = super::response::get_coverart::Response)
        )
    )]
    pub async fn get_coverart(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Query(params): axum::extract::Query<super::request::get_coverart::Params>,
    ) -> (
        axum::http::StatusCode,
        axum::Json<super::response::get_coverart::Response>,
    ) {
        let mut response = super::response::get_coverart::Response::default();

        match params.id {
            Some(id) => match super::cov_db::get_coverart(&pool, &id).await {
                Ok(coverart) => {
                    response.data.push(coverart);
                    response.message = String::from(super::super::response::SUCCESSFUL);
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

    /// Endpoint to download cover art
    #[utoipa::path(
        get,
        path = super::super::endpoints::DOWNLOADCOVERART,
        params(
            ("id" = uuid::Uuid, Path, description = "Cover art Id")
            ),
        responses(
            (status = 200, description = "Cover art downloading", body = Vec<u8>),
            (status = 404, description = "Cover art not found", body = Vec<u8>)
        )
    )]
    pub async fn download_coverart(
        axum::Extension(pool): axum::Extension<sqlx::PgPool>,
        axum::extract::Path(id): axum::extract::Path<uuid::Uuid>,
    ) -> (axum::http::StatusCode, axum::response::Response) {
        match super::cov_db::get_coverart(&pool, &id).await {
            Ok(coverart) => match icarus_models::coverart::io::to_data(&coverart) {
                Ok(data) => {
                    let bytes = axum::body::Bytes::from(data);
                    let mut response = bytes.into_response();
                    let headers = response.headers_mut();
                    // TODO: Address hard coding
                    headers.insert(
                        axum::http::header::CONTENT_TYPE,
                        "audio/jpg".parse().unwrap(),
                    );
                    headers.insert(
                        axum::http::header::CONTENT_DISPOSITION,
                        format!("attachment; filename=\"{id}.jpg\"")
                            .parse()
                            .unwrap(),
                    );

                    (axum::http::StatusCode::OK, response)
                }
                Err(_err) => (
                    axum::http::StatusCode::NOT_FOUND,
                    axum::response::Response::default(),
                ),
            },
            Err(_err) => (
                axum::http::StatusCode::NOT_FOUND,
                axum::response::Response::default(),
            ),
        }
    }
}
