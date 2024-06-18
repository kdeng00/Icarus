echo "Adding migrations..."
echo "Adding User migration"
dotnet-ef migrations add User --context UserContext
echo "Adding Album migration"
dotnet-ef migrations add Album --context AlbumContext
echo "Adding Artist migration"
dotnet-ef migrations add Artist --context ArtistContext
echo "Adding Genre migration"
dotnet-ef migrations add Genre --context GenreContext
echo "Adding Cover art migration"
dotnet-ef migrations add CoverArt --context CoverArtContext
echo "Adding Song migration"
dotnet-ef migrations add Song --context SongContext

echo "Updating migrations.."
echo "Updating User migration"
dotnet-ef database update --context UserContext
echo "Updating Album migration"
dotnet-ef database update --context AlbumContext
echo "Updating Artist migration"
dotnet-ef database update --context ArtistContext
echo "Updating Genre migration"
dotnet-ef database update --context GenreContext
echo "Updating Cover art migration"
dotnet-ef database update --context CoverArtContext
echo "Updating Song migration"
dotnet-ef database update --context SongContext
