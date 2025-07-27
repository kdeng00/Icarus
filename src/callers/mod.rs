pub mod coverart;
pub mod metadata;
pub mod song;

pub mod endpoints {
    pub const QUEUESONG: &str = "/api/v2/song/queue";
    pub const QUEUESONGLINKUSERID: &str = "/api/v2/song/queue/link";
    pub const QUEUESONGDATA: &str = "/api/v2/song/queue/{id}";
    pub const QUEUESONGUPDATE: &str = "/api/v2/song/queue/{id}";
    pub const NEXTQUEUESONG: &str = "/api/v2/song/queue/next";
    pub const QUEUEMETADATA: &str = "/api/v2/song/metadata/queue";
    pub const QUEUECOVERART: &str = "/api/v2/coverart/queue";
    pub const QUEUECOVERARTDATA: &str = "/api/v2/coverart/queue/data/{id}";
    pub const QUEUECOVERARTLINK: &str = "/api/v2/coverart/queue/link";
    pub const QUEUESONGDATAWIPE: &str = "/api/v2/song/queue/data/wipe";
    pub const QUEUECOVERARTDATAWIPE: &str = "/api/v2/coverart/queue/data/wipe";

    pub const CREATESONG: &str = "/api/v2/song";
    pub const GETSONGS: &str = "/api/v2/song";
    pub const STREAMSONG: &str = "/api/v2/song/stream";
    pub const CREATECOVERART: &str = "/api/v2/coverart";
    pub const GETCOVERART: &str = "/api/v2/coverart";
}

pub mod response {
    pub const SUCCESSFUL: &str = "SUCCESSFUL";
}
