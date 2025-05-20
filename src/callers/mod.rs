pub mod coverart;
pub mod metadata;
pub mod song;

pub mod endpoints {
    pub const QUEUESONG: &str = "/api/v2/song/queue";
    pub const QUEUESONGDATA: &str = "/api/v2/song/queue/{id}";
    pub const NEXTQUEUESONG: &str = "/api/v2/song/queue/next";
    pub const QUEUEMETADATA: &str = "/api/v2/song/metadata/queue";
    pub const QUEUECOVERART: &str = "/api/v2/coverart/queue";
    pub const QUEUECOVERARTLINK: &str = "/api/v2/coverart/queue/link";
}
