#ifndef SONG_DAO_H
#define SONG_DAO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <stdexcept>
#include <vector>

// CREATE TABLE IF NOT EXISTS songs (
//     id INTEGER PRIMARY KEY, 
//     title TEXT, 
//     artist TEXT, 
//     composer TEXT, 
//     album_id INTEGER, 
//     track INTEGER, 
//     disc INTEGER, 
//     date TEXT, 
//     copyright TEXT, 
//     genre TEXT, 
//     FOREIGN KEY (album_id) REFERENCES albums(id) ON DELETE SET NULL);

struct Song {
    int id;
    std::string title;
    std::string artist;
    std::string composer;
    int albumId;
    int track;
    int disc;
    std::string date;
    std::string copyright;
    std::string genre;
};

class SongDao {
public:
    SongDao(SQLite::Database& db): _db(db) {};
    
    std::vector<Song> getAllSongs();


    Song getSongById(int songId);

    int64_t insertSong(const Song& song);

    void deleteSongById(int songId);

    std::string getOldestSongDateInAlbum(int64_t albumId);

    std::string getMostFrequentGenreInAlbum(int64_t albumId);
    
    std::vector<std::tuple<int, std::string, std::string, int>> getTitleArtistAlbumTuples();

private:
    SQLite::Database& _db;
};

#endif // SONG_DAO_H
