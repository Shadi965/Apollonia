#ifndef DATABASE_H
#define DATABASE_H

#include "SQLiteCpp/SQLiteCpp.h"
#include <vector>
#include <string>
#include <unordered_map>

struct Song {
    int id;
    int ;
    std::string title;
    std::string artist;
    std::string albumArtist;
    std::string composer;
    std::string album;
    std::string track;
    std::string disc;
    std::string compilation;
    std::string date;
    std::string copyright;
    std::string genre;
    int duration;
    int bitrate;
    int channels;
    int sampleRate;
    std::string file;
    std::string isrc;

    static Song emptySong() {
        return Song({0, "", "", "", "", "", "", "", "", "", "", "", 0, 0, 0, 0, "", ""});
    }
};

class Database {
public:
    Database(const std::string& dbPath);
    std::vector<Song> getAllSongs();
    Song getSongById(int id);
    bool isSongFileExist(int id);
    void insertSong(Song song);
    
    void updateDatabase(const std::string& musicFolder, const std::string& jsonPath);

private:
    SQLite::Database db;
    int getNextAvailableId();
    std::unordered_map<std::string, std::pair<int, std::string>> loadJsonMetadata(const std::string& jsonPath);
    
};

#endif // DATABASE_H
