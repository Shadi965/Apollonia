#ifndef ALBUM_DAO_H
#define ALBUM_DAO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <stdexcept>
#include <vector>

// CREATE TABLE IF NOT EXISTS albums (
//     id INTEGER PRIMARY KEY AUTOINCREMENT, 
//     title TEXT, 
//     artist TEXT, 
//     track_count INTEGER, 
//     disc_count INTEGER, 
//     compilation INTEGER, 
//     date TEXT, 
//     copyright TEXT, 
//     genre TEXT, 
//     UNIQUE (title, artist));

struct Album {
    int id;
    std::string title;
    std::string artist;
    int track_count;
    int disc_count;
    bool compilation;
    std::string date;
    std::string copyright;
    std::string genre;
};

class AlbumDao {
public:
    AlbumDao(SQLite::Database& db): _db(db) {};


    std::vector<Album> getAllAlbums();

    Album getAlbumById(int albumId);

    std::string getAlbumTitleById(int albumId);


    int64_t isAlbumExists(const std::string& artist, const std::string& title);
    
    int64_t insertAlbum(const Album& album);

    std::vector<int> getAllAlbumsIds();
    
    int updateAlbumDateAndGenre(int albumId, const std::string& newDate, const std::string& newGenre);

private:
    SQLite::Database& _db;
};

#endif // ALBUM_DAO_H
