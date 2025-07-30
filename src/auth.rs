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



#[derive(Debug, Serialize, Deserialize)]
pub struct UserClaims {
    pub aud: String,    // Audience
    pub sub: String,         // Subject (user ID)
    pub exp: i64,            // Expiration time (UTC timestamp)
    pub iat: i64,            // Issued at (UTC timestamp)
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
    let token = cookie_jar
        .get("token")
        .map(|cookie| cookie.value().to_string())
        .or_else(|| {
            req.headers()
                .get(axum::http::header::AUTHORIZATION)
                .and_then(|auth_header| auth_header.to_str().ok())
                .and_then(|auth_value| {
                    if auth_value.starts_with("Bearer ") {
                        Some(auth_value[7..].to_owned())
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

    let claims = decode::<UserClaims>(
        &token,
        // TODO: Replace with code to get secret from env
        &DecodingKey::from_secret("my_super_secret".as_ref()),
        &Validation::default(),
    )
    .map_err(|_| {
        let json_error = ErrorResponse {
            status: "fail",
            message: "Invalid token".to_string(),
        };
        (StatusCode::UNAUTHORIZED, Json(json_error))
    })?
    .claims;

    let user_id = uuid::Uuid::parse_str(&claims.sub).map_err(|_| {
        let json_error = ErrorResponse {
            status: "fail",
            message: "Invalid token".to_string(),
        };
        (StatusCode::UNAUTHORIZED, Json(json_error))
    })?;

    req.extensions_mut().insert(user_id);
    Ok(next.run(req).await)
}
