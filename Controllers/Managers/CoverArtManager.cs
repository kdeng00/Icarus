using Icarus.Constants;
using Icarus.Controllers.Utilities;
using Icarus.Database.Contexts;
using Icarus.Models;
using Icarus.Types;

namespace Icarus.Controllers.Managers;

public class CoverArtManager : BaseManager
{
    #region Fields
    private string _rootCoverArtPath;
    private CoverArtContext _coverArtContext;
    private byte[] _stockCoverArt = null;
    #endregion


    #region Constructors
    public CoverArtManager(IConfiguration config)
    {
        _config = config;
        _connectionString = _config.GetConnectionString("DefaultConnection");
        _rootCoverArtPath = _config.GetValue<string>("CoverArtPath");
        Initialize();
    }
    #endregion


    #region Methods
    public void SaveCoverArtToDatabase(ref Song song, ref CoverArt coverArt)
    {
        _logger.Info("Saving cover art record to the database");
        _coverArtContext.Add(coverArt);
        _coverArtContext.SaveChanges();

        song.CoverArtID = coverArt.CoverArtID;
    }
    public void DeleteCoverArtFromDatabase(CoverArt coverArt)
    {
        _logger.Info("Attempting to delete cover art from the database");

        _coverArtContext.Attach(coverArt);
        _coverArtContext.Remove(coverArt);
        _coverArtContext.SaveChanges();
    }
    public void DeleteCoverArt(CoverArt coverArt)
    {
        try
        {
            var stockCoverArtPath = _rootCoverArtPath + "CoverArt.png";
            if (!string.Equals(stockCoverArtPath, coverArt.ImagePath, 
                        StringComparison.CurrentCultureIgnoreCase))
            {
                _logger.Info("Song does not contain the stock cover art");
                File.Delete(coverArt.ImagePath);
                _logger.Info("Cover art deleted from the filesystem");
            }
            else
            {
                _logger.Info("Song contains the stock cover art");
                _logger.Info("Will not delete from from the filesystem");
            }
        }
        catch (Exception ex)
        {
            var msg = ex.Message;
            _logger.Error(msg, "An error occurred");
        }
    }

    public CoverArt SaveCoverArt(Song song)
    {
        try
        {
            var dirMgr = new DirectoryManager(_rootCoverArtPath);
            var defaultExtension = ".png";
            dirMgr.CreateDirectory(song);

            var coverArt = new CoverArt
            {
                SongTitle = song.Title
            };

            var segment = coverArt.GenerateFilename(0);
            var imagePath = dirMgr.SongDirectory + segment + defaultExtension;

            coverArt.ImagePath = imagePath;

            var metaData = new MetadataRetriever();
            var imgBytes = metaData.RetrieveCoverArtBytes(song);
            
            if (imgBytes != null)
            {
                _logger.Info("Saving cover art to the filesystem");
                File.WriteAllBytes(coverArt.ImagePath, imgBytes);
            }
            else
            {
                _logger.Info("Song has no cover art, applying stock cover art");
                // coverArt.ImagePath = _rootCoverArtPath + $"{segment}{defaultExtension}";
                var coverArtFilePath = _rootCoverArtPath + $"{segment}{defaultExtension}";
                coverArt.ImagePath = DirectoryPaths.CoverArtPath;
                metaData.UpdateCoverArt(song, coverArt);
                coverArt.ImagePath = coverArtFilePath;
                File.WriteAllBytes(coverArt.ImagePath, _stockCoverArt);
            }

            return coverArt;
        }
        catch (Exception ex)
        {
            var msg = ex.Message;
            _logger.Error(msg, "An error occurred");
        }
        
        return null;
    }

    public CoverArt SaveCoverArt(IFormFile data, Song song)
    {
        var cover = new CoverArt { SongTitle = song.Title };

        try
        {
            var dirMgr = new DirectoryManager(_rootCoverArtPath);
            var defaultExtension = ".png";
            dirMgr.CreateDirectory(song);

            var segment = cover.GenerateFilename(0);
            var imagePath = dirMgr.SongDirectory + segment + defaultExtension;

            cover.ImagePath = imagePath;

            using (var fileStream = new FileStream(imagePath, FileMode.Create))
            {
                data.CopyTo(fileStream);
            }
        }
        catch (Exception ex)
        {
            _logger.Error(ex.Message, "An error occurred");
        }

        return cover;
    }

    public CoverArt GetCoverArt(Song song)
    {
        return _coverArtContext.CoverArtImages.FirstOrDefault(cov => cov.SongTitle.Equals(song.Title));
    }

    private void Initialize()
    {
        _coverArtContext = new CoverArtContext(_connectionString);

        if (System.IO.File.Exists(DirectoryPaths.CoverArtPath))
            _stockCoverArt = File.ReadAllBytes(DirectoryPaths.CoverArtPath);

        if (!File.Exists(_rootCoverArtPath + "CoverArt.png"))
        {
            File.WriteAllBytes(_rootCoverArtPath + "CoverArt.png", 
                    _stockCoverArt);
            Console.WriteLine("Copied Stock Cover Art");
        }
    }
    #endregion
}
