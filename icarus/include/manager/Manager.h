#ifndef MANAGER_H_
#define MANAGER_H_

#include <icarus_lib/icarus.h>

#include "manager/AlbumManager.h"
#include "manager/ArtistManager.h"
#include "manager/CoverArtManager.h"
#include "manager/DirectoryManager.h"
#include "manager/GenreManager.h"
#include "manager/SongManager.h"
#include "manager/TokenManager.h"
#include "manager/UserManager.h"
#include "manager/YearManager.h"

namespace manager
{
    using album_manager = AlbumManager;
    using artist_manager = ArtistManager;
    using genre_manager = GenreManager;
    using cover_art_manager = CoverArtManager;
    using directory_manager = DirectoryManager;
    using song_manager = SongManager;
    using token_manager = TokenManager;
    using user_manager = UserManager;
    using year_manager = YearManager;
}

#endif
