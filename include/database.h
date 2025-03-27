#ifndef DATABASE_H
#define DATABASE_H

#include "SQLiteCpp/SQLiteCpp.h"
#include <vector>
#include <string>

#include <list>
#include <map>

struct Song {
    int id;
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
    int itunesId;
    std::string file;
    int64_t lastModified;
};

class Database {
public:
    Database(const std::string& dbPath, const std::string& musicFolder, const std::string& jsonPath);
    std::vector<Song> getAllSongs();
    Song getSongById(int id);
    
    
    private:
    SQLite::Database db;
    
    void deleteSongsByIds(std::list<int>& ids);

    void checkUpdates(const std::string& musicFolder, const std::string& jsonPath);
    void filterFilesWithoutChanges(std::map<std::string, int64_t>& filesInFolder);
    void insertSongIntoDb(Song& song);
};

#endif // DATABASE_H
