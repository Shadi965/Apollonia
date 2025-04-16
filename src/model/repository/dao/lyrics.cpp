#include <sqlite3.h>

#include "lyrics.h"

LyricsDao::LyricsDao(SQLite::Database& db) : _db(db) {}

std::vector<LyricLineEntity> LyricsDao::getLyricsForSong(int songId) const {
    std::vector<LyricLineEntity> lyrics;
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

bool LyricsDao::addLyricLine(int songId, int timeMs, const std::string& line) {
    SQLite::Statement query(_db, "INSERT INTO lyrics (song_id, time_ms, line) VALUES (?, ?, ?)");
    query.bind(1, songId);
    query.bind(2, timeMs);
    query.bind(3, line);

    try {
        query.executeStep();
    }
    catch(const SQLite::Exception& e) {
        if (e.getExtendedErrorCode() == SQLITE_CONSTRAINT_UNIQUE || 
            e.getExtendedErrorCode() == SQLITE_CONSTRAINT_FOREIGNKEY)
            return false;
        throw e;
    }
    
    return _db.getChanges() != 0;
}
