// use std::collections::BTreeMap;
// use std::sync::Arc;

use axum::{
    // extract::State,
    http::{Request, StatusCode},
    middleware::Next,
    response::{IntoResponse},
    Json,
};
use axum_extra::extract::cookie::CookieJar;
use jsonwebtoken::{decode, DecodingKey, Validation};
use serde::{Deserialize, Serialize};
// use serde_json::{json, Value};
use thiserror::Error;
// use time::OffsetDateTime;


fn deserialize_i64_from_f64<'de, D>(deserializer: D) -> Result<i64, D::Error>
where
    D: serde::Deserializer<'de>,
{
    let val = f64::deserialize(deserializer)?;
    // Handle NaN and infinity cases
    if val.is_nan() || val.is_infinite() {
        return Err(serde::de::Error::custom("invalid float value"));
    }
    // Round to nearest integer and convert
    let rounded = val.round();
    // Check if the rounded value can fit in i64
    if rounded < (i64::MIN as f64) || rounded > (i64::MAX as f64) {
        return Err(serde::de::Error::custom("float out of i64 range"));
    }
    Ok(rounded as i64)
}

#[derive(Debug, Serialize, Deserialize)]
pub struct UserClaims {
    pub iss: String,
    pub aud: String,    // Audience
    pub sub: String,         // Subject (user ID)
    #[serde(deserialize_with = "deserialize_i64_from_f64")]
    pub exp: i64,            // Expiration time (UTC timestamp)
    #[serde(deserialize_with = "deserialize_i64_from_f64")]
    pub iat: i64,            // Issued at (UTC timestamp)
    // pub azp: String,
    // pub gty: String,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub roles: Option<Vec<String>>,  // Optional roles
}


#[derive(Error, Debug)]
pub enum JwtError {
    #[error("Token creation failed")]
    TokenCreation,
    #[error("Token verification failed")]
    TokenVerification,
    #[error("Invalid token")]
    InvalidToken,
    #[error("Token expired")]
    ExpiredToken,
    #[error("Invalid key")]
    InvalidKey,
}

/*
pub struct JwtAuth {
    key: Jwk,    // Now properly parameterized
}
*/


#[derive(Debug, Serialize)]
pub struct ErrorResponse {
    pub status: &'static str,
    pub message: String,
}

pub async fn auth<B>(
    cookie_jar: CookieJar,
    // State(data): State<Arc<AppState>>,
    mut req: Request<axum::body::Body>,
    next: Next,
) -> Result<impl IntoResponse, (StatusCode, Json<ErrorResponse>)> {
    println!("Cookie: {cookie_jar:?}");

    let token = cookie_jar
        .get("token")
        .map(|cookie| cookie.value().to_string())
        .or_else(|| {
            req.headers()
                .get(axum::http::header::AUTHORIZATION)
                .and_then(|auth_header| auth_header.to_str().ok())
                .and_then(|auth_value| {
                    println!("Auth value: {auth_value:?}");
                    if let Some(stripped) = auth_value.strip_prefix("Bearer ") {
                        Some(String::from(stripped))
                    } else {
                        None
                    }
                })
        });

    let token = token.ok_or_else(|| {
        let json_error = ErrorResponse {
            status: "fail",
            message: "You are not logged in, please provide token".to_string(),
        };
        (StatusCode::UNAUTHORIZED, Json(json_error))
    })?;

    let secret_key = icarus_envy::environment::get_secret_main_key().await;

    let mut validation = Validation::new(jsonwebtoken::Algorithm::HS256);
    validation.set_audience(&["icarus"]); // Must match exactly what's in the token
    let claims = decode::<UserClaims>(
        &token,
        // TODO: Replace with code to get secret from env
        &DecodingKey::from_secret(secret_key.as_ref()),
        &validation,
    )
    .map_err(|err| {
        eprintln!("Error: {err:?}");
        let json_error = ErrorResponse {
            status: "fail",
            message: "Invalid token - Error decoding claims".to_string(),
        };
        (StatusCode::UNAUTHORIZED, Json(json_error))
    })?
    .claims;

    println!("Claims: {claims:?}");

    /*
    let user_id = uuid::Uuid::parse_str(&claims.sub).map_err(|_| {
        let json_error = ErrorResponse {
            status: "fail",
            message: "Invalid token".to_string(),
        };
        (StatusCode::UNAUTHORIZED, Json(json_error))
    })?;
    */

    // req.extensions_mut().insert(user_id);
    Ok(next.run(req).await)
}
