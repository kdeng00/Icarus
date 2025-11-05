use axum::{
    Json,
    http::{Request, StatusCode},
    middleware::Next,
    response::IntoResponse,
};
use axum_extra::extract::cookie::CookieJar;
use jsonwebtoken::{DecodingKey, Validation, decode};
use serde::Serialize;

// TODO: Put this in icarus_models
/*
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
*/

// TODO: Put this in icarus_models
/*
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct UserClaims {
    pub iss: String,
    pub aud: String, // Audience
    pub sub: String, // Subject (user ID)
    #[serde(deserialize_with = "deserialize_i64_from_f64")]
    pub exp: i64, // Expiration time (UTC timestamp)
    #[serde(deserialize_with = "deserialize_i64_from_f64")]
    pub iat: i64, // Issued at (UTC timestamp)
    // pub azp: String,
    // pub gty: String,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub roles: Option<Vec<String>>, // Optional roles
}
*/

#[derive(Debug, Serialize)]
pub struct ErrorResponse {
    pub status: &'static str,
    pub message: String,
}

pub async fn auth<B>(
    cookie_jar: CookieJar,
    req: Request<axum::body::Body>,
    next: Next,
) -> Result<impl IntoResponse, (StatusCode, Json<ErrorResponse>)> {
    let token = cookie_jar
        .get("token")
        .map(|cookie| cookie.value().to_string())
        .or_else(|| {
            req.headers()
                .get(axum::http::header::AUTHORIZATION)
                .and_then(|auth_header| auth_header.to_str().ok())
                .and_then(|auth_value| auth_value.strip_prefix("Bearer ").map(String::from))
        });

    let token = token.ok_or_else(|| {
        let json_error = ErrorResponse {
            status: "fail",
            message: "You are not logged in, please provide token".to_string(),
        };
        (StatusCode::UNAUTHORIZED, Json(json_error))
    })?;

    let secret_key = icarus_envy::environment::get_secret_main_key().await.value;

    let mut validation = Validation::new(jsonwebtoken::Algorithm::HS256);
    validation.set_audience(&["icarus"]); // Must match exactly what's in the token
    let _claims = decode::<icarus_models::token::UserClaims>(
        &token,
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

    Ok(next.run(req).await)
}
