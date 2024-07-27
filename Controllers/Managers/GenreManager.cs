using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.Managers;

public class GenreManager : BaseManager
{
    #region Fields
    private GenreContext? _genreContext;
    #endregion


    #region Properties
    #endregion


    #region Constructors
    public GenreManager(IConfiguration config)
    {
        _config = config;
        _connectionString = _config.GetConnectionString("DefaultConnection");
        _genreContext = new GenreContext(_connectionString!);
    }
    #endregion


    #region Methods
    public void SaveGenreToDatabase(ref Song song)
    {
        _logger.Info("Starting process to save the genre record of the song to the database");

        var genre = new Genre
        {
            GenreName = song.Genre,
            SongCount = 1
        };

        var genreName = song.Genre;
        var genreRetrieved = _genreContext!.Genres!.FirstOrDefault(gnr => gnr.GenreName!.Equals(genreName));

        if (genreRetrieved == null)
        {
            _genreContext.Add(genre);
            _genreContext.SaveChanges();
        }
        else
        {
            genre.Id = genreRetrieved.Id;
        }

        song.GenreId = genre.Id;
    }

    public Genre UpdateGenreInDatabase(Song oldSongRecord, Song newSongRecord)
    {
        var oldGenreRecord = _genreContext!.Genres!.FirstOrDefault(gnr => gnr.GenreName!.Equals(oldSongRecord.Genre));
        var oldGenreName = oldGenreRecord!.GenreName;
        var newGenreName = newSongRecord.Genre;

        if (string.IsNullOrEmpty(newGenreName) || oldGenreName!.Equals(newGenreName))
        {
            _logger.Info("No change to the song's Genre");
            return oldGenreRecord;
        }

        _logger.Info("Change to the song's genre found");

        if (oldGenreRecord.SongCount <= 1)
        {
            _logger.Info("Deleting genre record");

            _genreContext.Remove(oldGenreRecord);
            _genreContext.SaveChanges();
        }

        if (!(_genreContext.Genres!.FirstOrDefault(gnr => gnr.GenreName!.Equals(oldSongRecord.Genre)) != null))
        {
            _logger.Info("Creating new genre record");

            var newGenreRecord = new Genre
            {
                GenreName = newGenreName
            };

            _genreContext.Add(newGenreRecord);
            _genreContext.SaveChanges();

            return newGenreRecord;
        }
        else
        {
            _logger.Info("Updating existing genre record");

            var existingGenreRecord = _genreContext.Genres!.FirstOrDefault(gnr => gnr.GenreName!.Equals(oldGenreRecord.GenreName));

            _genreContext.Update(existingGenreRecord!);
            _genreContext.SaveChanges();

            return existingGenreRecord!;
        }
    }

    public void DeleteGenreFromDatabase(Song song)
    {
        if (!(_genreContext!.Genres!.FirstOrDefault(gnr => gnr.GenreName!.Equals(song.Genre)) != null))
        {
            _logger.Info("Cannot delete the genre record because it does not exist");
            return;
        }

        var genre = _genreContext.Genres!.FirstOrDefault(gnr => gnr.GenreName!.Equals(song.Genre));

        if (SongsInGenre(genre!) <= 1)
        {
            _genreContext.Remove(genre!);
            _genreContext.SaveChanges();
        }
    }

    private int SongsInGenre(Genre genre)
    {
        var sngContext = new SongContext(_connectionString!);
        var songs = sngContext.Songs!.Where(sng => sng.GenreId == genre.Id).ToList();

        return songs.Count;
    }
    #endregion
}
