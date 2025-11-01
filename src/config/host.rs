/// Full hosting address that the application will be broadcasting
pub fn get_full() -> String {
    format!("{ADDRESS}:{PORT}")
}

pub const ADDRESS: &str = "0.0.0.0";
pub const PORT: &str = "8080";
