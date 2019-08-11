
struct Song
{
    int Id;
    char Title[1024];
    char Artist[1024];
    char Album[1024];
    char Genre[1024];
    int Year;
    int Duration;
    char SongPath[1024];
};

struct Cover
{
    int Id;
    char SongTitle[1024];
    char ImagePath[1024];
};
