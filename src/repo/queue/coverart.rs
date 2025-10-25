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
) -> Result<crate::callers::queue::coverart::CoverArtQueue, sqlx::Error> {
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
        Ok(row) => Ok(crate::callers::queue::coverart::CoverArtQueue {
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
) -> Result<crate::callers::queue::coverart::CoverArtQueue, sqlx::Error> {
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
        Ok(row) => Ok(crate::callers::queue::coverart::CoverArtQueue {
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
