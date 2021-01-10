#ifndef REPOSITORIES_H_
#define REPOSITORIES_H_


#include "database/AlbumRepository.h"
#include "database/ArtistRepository.h"
#include "database/BaseRepository.h"
#include "database/CoverArtRepository.h"
#include "database/GenreRepository.h"
#include "database/SongRepository.h"
#include "database/TokenRepository.h"
#include "database/UserRepository.h"
#include "database/YearRepository.h"


namespace database
{

    using album_repo = AlbumRepository;
    using artist_repo = ArtistRepository;
    using base_repo = BaseRepository;
    using cover_art_repository = CoverArtRepository;
    using genre_repository = GenreRepository;
    using song_repo = SongRepository;
    using token_repo = TokenRepository;
    using user_repo = UserRepository;
    using year_repo = YearRepository;
}


#endif
