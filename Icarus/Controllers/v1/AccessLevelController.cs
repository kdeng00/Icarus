using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

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
    public IActionResult GetAccessLevels(int? id, int? songId)
    {
        var accLevel = new Icarus.Models.AccessLevel { Id = 0 };
        var accessLevelContext = new Icarus.Database.Contexts.AccessLevelContext(_connectionString!);

        if (id != null)
        {
            accLevel = accessLevelContext.AccessLevels!.FirstOrDefault(al => al.Id == id);
        }
        else if (songId != null)
        {
            accLevel = accessLevelContext.AccessLevels!.FirstOrDefault(al => al.SongId == songId);
        }

        var response = new GetAccessLevelsResponse { Data = new List<Models.AccessLevel>() };

        if (accLevel?.Id > 0)
        {
            response.Subject = "Successful";
            response.Data.Add(accLevel);
            return Ok(response);
        }
        else
        {
            response.Subject = "Failure";
            return BadRequest(response);
        }
    }

    [HttpPatch("{id}")]
    public IActionResult UpdateAccessLevel(int id, [FromBody] Models.AccessLevel accessLevel)
    {
        var response = new UpdateAccessLevelResponse { Data = new List<Models.AccessLevel>() };
        var targetLevel = accessLevel.Level;
        if (targetLevel == null)
        {
            response.Subject = "No level provided";
            return BadRequest(response);
        }
        else
        {
            if (!Models.AccessLevel.IsAccessLevelValid(targetLevel))
            {
                response.Subject = "Invalid level";
                return BadRequest(response);
            }
        }

        var accessLevelContext = new Database.Contexts.AccessLevelContext(this._connectionString!);

        var fetchedAccLevel = accessLevelContext.AccessLevels!.FirstOrDefault(al => al.Id == id);

        if (fetchedAccLevel == null)
        {
            response.Subject = "Nothing found";
            return NotFound(response);
        }

        var fetchedLevel = fetchedAccLevel!.Level;

        if (fetchedLevel!.Equals(targetLevel))
        {
            // No change
            response.Subject = "No change";
            response.Data.Add(fetchedAccLevel);
            return Ok(response);
        }
        else
        {
            fetchedAccLevel.Level = targetLevel;
            response.Subject = "Successful";
            accessLevelContext.Update(fetchedAccLevel);
            accessLevelContext.SaveChanges();

            response.Data.Add(fetchedAccLevel);
            return Ok(response);
        }
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
