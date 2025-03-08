using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

// using I
// using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1;

[Route("api/v1/accesslevel")]
[ApiController]
[Authorize]
public class AccessLevelController : BaseController
{
    #region Fields
    private readonly ILogger<AccessLevelController>? _logger;
    private string? _connectionString;
    #endregion

    #region Properties
    #endregion

    #region Constructors
    public AccessLevelController(ILogger<AccessLevelController> logger, IConfiguration config)
    {
        this._logger = logger;
        this._config = config;
        _connectionString = this._config.GetConnectionString("DefaultConnection");
    }
    #endregion

    #region HTTP Routes
    [HttpGet]
    public IActionResult GetAccessLevels([FromBody] Icarus.Models.AccessLevel accessLevel)
    {
        var accLevel = new Icarus.Models.AccessLevel { Id = 0 };
        var accessLevelContext = new Icarus.Database.Contexts.AccessLevelContext(_connectionString!);

        if (accessLevel.Id != 0)
        {
            var id = accessLevel.Id;
            accLevel = accessLevelContext.AccessLevels!.FirstOrDefault(al => al.Id == id);
        }
        else if (accessLevel.SongId != 0)
        {
            var songId = accessLevel.SongId;
            accLevel = accessLevelContext.AccessLevels!.FirstOrDefault(al => al.SongId == songId);
        }

        var response = new GetAccessLevelsResponse();

        if (accLevel?.Id > 0)
        {
            response.Subject = "Successful";
            response.Data = new List<Icarus.Models.AccessLevel>();
            response.Data.Add(accLevel);
        }
        else
        {
            response.Subject = "Failure";
        }

        return Ok(response);
    }

    public IActionResult UpdateAccessLevel()
    {
        return Ok(new UpdateAccessLevelResponse());
    }
    #endregion
}

#region Responses
public class GetAccessLevelsResponse
{
    #region Properties
    [Newtonsoft.Json.JsonProperty("subject")]
    public string? Subject { get; set; }
    [Newtonsoft.Json.JsonProperty("data")]
    public List<Icarus.Models.AccessLevel>? Data { get; set; }
    #endregion
}

public class UpdateAccessLevelResponse
{
    #region Properties
    [Newtonsoft.Json.JsonProperty("subject")]
    public string? Subject { get; set; }
    [Newtonsoft.Json.JsonProperty("data")]
    public List<Icarus.Models.AccessLevel>? Data { get; set; }
    #endregion
}
#endregion
