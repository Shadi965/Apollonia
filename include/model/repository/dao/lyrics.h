#ifndef LYRICS_DAO_H
#define LYRICS_DAO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include <stdexcept>

// CREATE TABLE IF NOT EXISTS lyrics (
//     song_id INTEGER NOT NULL, 
//     time_ms INTEGER NOT NULL, 
//     line TEXT NOT NULL, 
//     FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE);
//
// CREATE INDEX idx_lyrics_song_time ON lyrics(song_id, time_ms);

struct LyricLine {
    int songId;
    int timeMs;
    std::string line;
};

class LyricsDao {
public:
    LyricsDao(SQLite::Database& db) : _db(db) {}

    void addLyricLine(int songId, int timeMs, const std::string& line);

    std::vector<LyricLine> getLyricsForSong(int songId);

private:
    SQLite::Database& _db;
};

#endif // LYRICS_DAO_H
