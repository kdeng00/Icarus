pub mod coverart;
pub mod metadata;
pub mod queue;
pub mod song;

pub mod endpoints {
    pub const QUEUEMETADATA: &str = "/api/v2/song/metadata/queue";

    pub const CREATESONG: &str = "/api/v2/song";
    pub const GETSONGS: &str = "/api/v2/song";
    pub const GETALLSONGS: &str = "/api/v2/song/all";
    pub const STREAMSONG: &str = "/api/v2/song/stream/{id}";
    pub const DOWNLOADSONG: &str = "/api/v2/song/download/{id}";
    pub const DELETESONG: &str = "/api/v2/song/{id}";
    pub const CREATECOVERART: &str = "/api/v2/coverart";
    pub const GETCOVERART: &str = "/api/v2/coverart";
    pub const DOWNLOADCOVERART: &str = "/api/v2/coverart/download/{id}";
}

pub mod response {
    pub const SUCCESSFUL: &str = "SUCCESSFUL";
}
