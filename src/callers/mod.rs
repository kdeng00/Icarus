pub mod metadata;
pub mod song;

pub mod endpoints {
    pub const QUEUESONG: &str = "/api/v2/song/queue";
    pub const NEXTQUEUESONG: &str = "/api/v2/song/queue/next";
    pub const QUEUEMETADATA: &str = "/api/v2/song/metadata/queue";
}
