#ifndef SCOPES_H_
#define SCOPES_H_

namespace type {
    enum class Scope {
        upload = 0,
        download,
        stream,
        retrieveSong,
        updateSong,
        deleteSong,
        retrieveAlbum,
        retrieveArtist,
        retrieveGenre,
        retrieveYear,
        downloadCoverArt

    };
}

#endif
