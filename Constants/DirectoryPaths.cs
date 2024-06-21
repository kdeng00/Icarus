namespace Icarus.Constants;

public class DirectoryPaths
{
    // TODO: Remove this
    public static string CoverArtPath =>
        Directory.GetCurrentDirectory() + "/Images/Stock/CoverArt.png";
    public static string CoverArtDirectory =>
        Directory.GetCurrentDirectory() + "/Images/Stock/";
    public static string CoverArtFilename => "CoverArt.png";
    public static string FILENAME_CHARACTERS = "ABCDEF0123456789";
    public static int FILENAME_LENGTH = 25;
}
