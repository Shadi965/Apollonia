#include "lyrics.h"

// CREATE TABLE IF NOT EXISTS lyrics (
//     song_id INTEGER NOT NULL, 
//     time_ms INTEGER NOT NULL, 
//     line TEXT NOT NULL, 
//     FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE);
//
// CREATE INDEX idx_lyrics_song_time ON lyrics(song_id, time_ms);

void LyricsDao::addLyricLine(int songId, int timeMs, const std::string& line) {
    SQLite::Statement query(_db, "INSERT INTO lyrics (song_id, time_ms, line) VALUES (?, ?, ?)");
    query.bind(1, songId);
    query.bind(2, timeMs);
    query.bind(3, line);
    query.executeStep();
}

std::vector<LyricLine> LyricsDao::getLyricsForSong(int songId) {
    std::vector<LyricLine> lyrics;
    SQLite::Statement query(_db, "SELECT time_ms, line FROM lyrics WHERE song_id = ? ORDER BY time_ms ASC");
    query.bind(1, songId);

    while (query.executeStep()) {
        lyrics.push_back({
            songId,
            query.getColumn(0).getInt(),
            query.getColumn(1).getString()
        });
    }

    return lyrics;
}
