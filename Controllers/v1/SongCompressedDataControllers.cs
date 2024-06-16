using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;

using Icarus.Controllers.Managers;
using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1;

[Route("api/v1/song/compressed/data")]
[ApiController]
[Authorize]
public class SongCompressedDataController : BaseController
{
    #region Fields
    private string _connectionString;
    private string _songTempDir;
    private string _archiveDir;
    #endregion


    #region Properties
    #endregion


    #region Constructor
    public SongCompressedDataController(IConfiguration config)
    {
        _config = config;
        _songTempDir = _config.GetValue<string>("TemporaryMusicPath");
        _archiveDir = _config.GetValue<string>("ArchivePath");
        _connectionString = _config.GetConnectionString("DefaultConnection");
    }
    #endregion


    #region API Routes
    [HttpGet("{id}")]
    public async Task<IActionResult> DownloadCompressedSong(int id, [FromQuery] bool? randomizeFilename)
    {
        var context = new SongContext(_connectionString);

        SongCompression cmp = new SongCompression(_archiveDir);

    
        Console.WriteLine($"Archive directory root: {_archiveDir}");

        Console.WriteLine("Starting process of retrieving comrpessed song");
        var sng = context.RetrieveRecord(new Song{ SongID = id });
        SongData song = await cmp.RetrieveCompressedSong(sng);

        var filename = string.Empty;

        if (randomizeFilename.HasValue && randomizeFilename.Value) 
        {
            filename = Managers.DirectoryManager.GenerateFilename(10) + Constants.FileExtensions.ZIP_EXTENSION;
        }
        else
        {
            filename = sng.Title + Constants.FileExtensions.ZIP_EXTENSION;
        }

        return File(song.Data, "application/x-msdownload", filename);
    }
    #endregion
}
