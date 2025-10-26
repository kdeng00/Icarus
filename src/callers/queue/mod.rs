pub mod coverart;
pub mod song;

pub mod endpoints {
    pub const QUEUESONG: &str = "/api/v2/song/queue";
    pub const QUEUESONGLINKUSERID: &str = "/api/v2/song/queue/link";
    pub const QUEUESONGDATA: &str = "/api/v2/song/queue/{id}";
    pub const QUEUESONGUPDATE: &str = "/api/v2/song/queue/{id}";
    pub const NEXTQUEUESONG: &str = "/api/v2/song/queue/next";
    pub const QUEUECOVERART: &str = "/api/v2/coverart/queue";
    pub const QUEUECOVERARTDATA: &str = "/api/v2/coverart/queue/data/{id}";
    pub const QUEUECOVERARTLINK: &str = "/api/v2/coverart/queue/link";
    pub const QUEUESONGDATAWIPE: &str = "/api/v2/song/queue/data/wipe";
    pub const QUEUECOVERARTDATAWIPE: &str = "/api/v2/coverart/queue/data/wipe";
}
