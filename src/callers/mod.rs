pub mod metadata;
pub mod song;

pub mod endpoints {
    pub const QUEUESONG: &str = "/api/v2/song/queue";
    pub const QUEUEMETADATA: &str = "/api/v2/song/metadata/queue";
}
