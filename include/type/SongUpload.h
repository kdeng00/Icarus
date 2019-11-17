#ifndef SONGUPLOAD_H_
#define SONGUPLOAD_H_

namespace type {
    enum class SongUpload {
        Successful = 0,
        AlreadyExist = 1,
        Failed = 2
    };
}

#endif
