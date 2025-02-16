using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.Managers;

public class ArtistManager : BaseManager
{
    #region Fields
    private ArtistContext? _artistContext;
    #endregion


    #region Properties
    #endregion


    #region Constructors
    public ArtistManager(IConfiguration config)
    {
        _config = config;
        _connectionString = _config.GetConnectionString("DefaultConnection");
        _artistContext = new ArtistContext(_connectionString!);
    }
    #endregion


    #region Methods
    public void SaveArtistToDatabase(ref Song song)
    {
        _logger.Info("Starting process to save the artist record of the song to the database");

        var artist = new Artist
        {
            Name = song.Artist,
            SongCount = 1
        };

        var artistRetrieved = _artistContext!.Artists.FirstOrDefault(art => art.Name!.Equals(artist.Name));

        if (artistRetrieved == null)
        {
            artist.SongCount = 1;
            _artistContext.Add(artist);
            _artistContext.SaveChanges();
        }
        else
        {
            artist.Id = artistRetrieved.Id;
        }

        song.ArtistId = artist.Id;
    }

    public Artist UpdateArtistInDatabase(Song oldSongRecord, Song newSongRecord)
    {
        var oldArtistRecord = _artistContext!.Artists.FirstOrDefault(art => art.Name!.Equals(oldSongRecord.AlbumTitle));
        var oldArtistName = oldArtistRecord!.Name;
        var newArtistName = newSongRecord.Artist;

        if (string.IsNullOrEmpty(newArtistName) || oldArtistName!.Equals(newArtistName))
        {
            _logger.Info("No change to the song's Artist");
            return oldArtistRecord;
        }

        _logger.Info("Change to the song's record found");

        if (oldArtistRecord.SongCount <= 1)
        {
            _logger.Info("Deleting artist record that no longer has any songs");

            _artistContext.Remove(oldArtistRecord);
            _artistContext.SaveChanges();
        }

        if (!(_artistContext.Artists.FirstOrDefault(art => art.Name!.Equals(oldSongRecord.AlbumTitle)) != null))
        {
            _logger.Info("Creating new artist record");

            var newArtistRecord = new Artist
            {
                Name = newArtistName
            };

            _artistContext.Add(newArtistRecord);
            _artistContext.SaveChanges();

            return newArtistRecord;
        }
        else
        {
            _logger.Info("Updating existing artist record");

            var existingArtistRecord = _artistContext.Artists.FirstOrDefault(art => art.Name!.Equals(newSongRecord.AlbumTitle));

            _artistContext.Update(existingArtistRecord!);
            _artistContext.SaveChanges();

            return existingArtistRecord!;
        }
    }

    public void DeleteArtistFromDatabase(Song song)
    {
        if (!(_artistContext!.Artists.FirstOrDefault(art => art.Name!.Equals(song.Artist)) != null))
        {
            _logger.Info("Cannot delete the artist record because it does not exist");
            return;
        }

        var artist = _artistContext.Artists.FirstOrDefault(art => art.Name!.Equals(song.Artist));

        if (SongsOfArtist(artist!) <= 1)
        {
            _artistContext!.Remove(artist!);
            _artistContext.SaveChanges();
        }
    }

    private int SongsOfArtist(Artist artist)
    {
        var sngContext = new SongContext(_connectionString!);
        var songs = sngContext.Songs!.Where(sng => sng.ArtistId == artist.Id).ToList();

        return songs.Count;
    }
    #endregion
}
