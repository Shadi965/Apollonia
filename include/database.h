#ifndef DATABASE_H
#define DATABASE_H

#include "SQLiteCpp/SQLiteCpp.h"
#include <vector>
#include <string>

#include <list>
#include <map>

extern "C" {
#include <libavformat/avformat.h>
}

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
    Database(const std::string& dbPath);
    std::vector<Song> getAllSongs();
    Song getSongById(int id);
    
    void updateDatabase(const std::string& musicFolder, const std::string& jsonPath);

private:
    SQLite::Database db;

    std::map<std::string, int> loadJsonMetadata(const std::string& jsonPath);
    
    int64_t last_write_time(const std::string& path);
    std::map<std::string, int64_t> filesChangedDatesFromDir(const std::string& dir);
    void deleteSongsByIds(std::list<int>& ids);
    void filterFilesWithoutChanges(std::map<std::string, int64_t>& filesInFolder);
    Song extractMetadata(AVFormatContext* formatCtx, std::pair<std::string, int64_t> fileChangedDate, std::map<std::string, int>& itunesIds);
    void insertSongIntoDb(Song& song);
    void findItunesId(Song& song, std::map<std::string, int>& itunesIds);
};

#endif // DATABASE_H
