#ifndef CONTROLLERS_H_
#define CONTROLLERS_H_

#include "controller/AlbumController.hpp"
#include "controller/ArtistController.hpp"
#include "controller/CoverArtController.hpp"
#include "controller/GenreController.hpp"
#include "controller/LoginController.hpp"
#include "controller/RegisterController.hpp"
#include "controller/SongController.hpp"
#include "controller/YearController.hpp"

namespace controller
{
    using album_controller = AlbumController;
    using artist_controller = ArtistController;
    using cover_art_controller = CoverArtController;
    using genre_controller = GenreController;
    using login_controller = LoginController;
    using register_controller = RegisterController;
    using song_controller = SongController;
    using year_controller = YearController;
}

#endif
