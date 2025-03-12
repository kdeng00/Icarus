echo "Adding migrations..."

echo "Adding User migration"
dotnet dotnet-ef migrations add User --context UserContext
echo "Adding Album migration"
dotnet dotnet-ef migrations add Album --context AlbumContext
echo "Adding Artist migration"
dotnet dotnet-ef migrations add Artist --context ArtistContext
echo "Adding Genre migration"
dotnet dotnet-ef migrations add Genre --context GenreContext
echo "Adding Cover art migration"
dotnet dotnet-ef migrations add CoverArt --context CoverArtContext
echo "Adding Song migration"
dotnet dotnet-ef migrations add Song --context SongContext
echo "Adding AccessLevel migration" 
dotnet dotnet-ef migrations add AccessLevel --context AccessLevelContext

echo "Updating migrations.."
echo "Updating User migration"
dotnet dotnet-ef database update --context UserContext
echo "Updating Album migration"
dotnet dotnet-ef database update --context AlbumContext
echo "Updating Artist migration"
dotnet dotnet-ef database update --context ArtistContext
echo "Updating Genre migration"
dotnet dotnet-ef database update --context GenreContext
echo "Updating Cover art migration"
dotnet dotnet-ef database update --context CoverArtContext
echo "Updating Song migration"
dotnet dotnet-ef database update --context SongContext
echo "Updating AccessLevel migration"
dotnet dotnet-ef database update --context AccessLevelContext

