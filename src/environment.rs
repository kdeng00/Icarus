pub async fn get_db_url() -> String {
    dotenvy::dotenv().ok();
    std::env::var(crate::keys::DBURL).expect(crate::keys::error::ERROR)
}

pub async fn get_root_directory() -> Result<String, std::env::VarError> {
    dotenvy::dotenv().ok();
    std::env::var(crate::keys::ROOT_DIRECTORY)
}
