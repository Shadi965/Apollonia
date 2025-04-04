#ifndef SONG_META_DAO_H
#define SONG_META_DAO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include <stdexcept>

// CREATE TABLE IF NOT EXISTS songs_meta (
//     song_id INTEGER, 
//     duration INTEGER, 
//     bitrate INTEGER, 
//     channels INTEGER, 
//     sample_rate INTEGER, 
//     last_modified INTEGER NOT NULL, 
//     file TEXT UNIQUE, 
//     FOREIGN KEY (song_id) REFERENCES songs(id) ON DELETE CASCADE);

struct SongMeta {
    int songId;
    int duration;
    int bitrate;
    int channels;
    int sampleRate;
    int lastModified;
    std::string file;
};

class SongMetaDao {
public:
    SongMetaDao(SQLite::Database& db): _db(db) {};


    std::vector<SongMeta> getAllSongsData();

    SongMeta getSongDataById(int songId);

    int getSongDuration(int songId);

    int64_t insertSongData(const SongMeta& meta);

    std::string getFilePathBySongId(int songId);

private:
    SQLite::Database& _db;
};

#endif // SONG_META_DAO_H
