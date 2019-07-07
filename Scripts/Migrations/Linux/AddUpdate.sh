echo "Adding migrations..."
echo "Adding User migration"
dotnet ef migrations add User --context UserContext
echo "Adding Song migration"
dotnet ef migrations add Song --context SongContext
echo "Adding Album migration"
dotnet ef migrations add Album --context AlbumContext
echo "Adding Artist migration"
dotnet ef migrations add Artist --context ArtistContext
echo "Adding Genre migration"
dotnet ef migrations add Genre --context GenreContext
echo "Adding Year migration"
dotnet ef migrations add Year --context YearContext
echo "Adding Cover art migration"
dotnet ef migrations add CoverArt --context CoverArtContext

echo "Updating migrations.."
echo "Updating User migration"
dotnet ef database update --context UserContext
echo "Updating Song migration"
echo "Updating Album migration"
echo "Updating Artist migration"
echo "Updating Genre migration"
echo "Updating Year migration"
echo "Updating Cover art migration"
dotnet ef database update --context SongContext
