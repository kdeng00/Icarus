use std::collections::BTreeMap;


use axum::{
    http::{Request, StatusCode},
    middleware::Next,
    response::{IntoResponse, Response},
    Json,
};
use josekit::{
    jwt::{self, JwtPayload},
    jws::{self, JwsHeader, JwsSigner, JwsVerifier},
    jwk::Jwk,
};
use serde::{Deserialize, Serialize};
use serde_json::{json, Value};
use thiserror::Error;
use time::OffsetDateTime;

#[derive(Debug, Serialize, Deserialize)]
pub struct UserClaims {
    pub sub: String,         // Subject (user ID)
    pub exp: i64,            // Expiration time (UTC timestamp)
    pub iat: i64,            // Issued at (UTC timestamp)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub roles: Option<Vec<String>>,  // Optional roles
}


// use time::OffsetDateTime;

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

pub struct JwtAuth {
    key: Jwk,    // Now properly parameterized
}

impl JwtAuth {
    pub fn new(secret: &str) -> Result<Self, JwtError> {
        let mut key = Jwk::new("oct");
        key.set_parameter("k", Some(json!(base64::encode(secret))))
            .map_err(|_| JwtError::InvalidKey)?;
        Ok(Self { key })
    }

    pub fn create_token(&self, claims: &UserClaims) -> Result<String, JwtError> {
        let mut header = JwsHeader::new();
        header.set_token_type("JWT");

        let claims_value = serde_json::to_value(claims)
            .map_err(|_| JwtError::SerializationError)?;
        let claims_map = claims_value.as_object()
            .ok_or(JwtError::SerializationError)?
            .to_owned();

        let mut payload = JwtPayload::new();
        for (k, v) in claims_map {
            payload.set_claim(&k, Some(v))
                .map_err(|_| JwtError::TokenCreation)?;
        }

        let signer = jws::HS256.signer_from_jwk(&self.key)
            .map_err(|_| JwtError::TokenCreation)?;

        jwt::encode_with_signer(&payload, &header, &signer)
            .map_err(|_| JwtError::TokenCreation)
    }

    pub fn verify_token(&self, token: &str) -> Result<UserClaims, JwtError> {
        let verifier = jws::HS256.verifier_from_jwk(&self.key)
            .map_err(|_| JwtError::TokenVerification)?;

        let (payload, _) = jwt::decode_with_verifier(token, &verifier)
            .map_err(|_| JwtError::InvalidToken)?;

        // Convert payload to JSON value
        let mut claims_value = json!({});
        if let Some(Value::Object(map)) = claims_value.as_object_mut() {
            for key in payload.claim_names() {
                if let Some(value) = payload.claim(key) {
                    map.insert(key.to_string(), value.clone());
                }
            }
        }

        let claims = serde_json::from_value(claims_value)
            .map_err(|_| JwtError::InvalidToken)?;

        // Check expiration
        if let Some(Value::Number(exp)) = payload.claim("exp") {
            if let Some(exp) = exp.as_i64() {
                let now = OffsetDateTime::now_utc().unix_timestamp();
                if exp < now {
                    return Err(JwtError::ExpiredToken);
                }
            }
        }

        Ok(claims)
    }
}
