namespace Icarus.Models;

public class SongData
{
    #region Properties
    public Guid ID { get; set; }
    public byte[]? Data { get; set; }
    public int SongID { get; set; }
    #endregion
}
