use sqlx::Row;

#[derive(Debug, serde::Serialize, sqlx::FromRow)]
pub struct InsertedData {
    pub id: uuid::Uuid,
}

#[derive(Debug, serde::Deserialize, serde::Serialize, sqlx::FromRow, utoipa::ToSchema)]
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
