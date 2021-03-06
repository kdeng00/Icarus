#ifndef SONGFILTER_H_
#define SONGFILTER_H_

namespace type {
    enum class SongFilter {
        id = 0,
        title,
        album,
        artist,
        albumArtist,
        genre,
        year,
        titleAndArtist,
        titleAlbArtistAlbum,
        titleAlbArtistAlbumTrack
    };
}

#endif
