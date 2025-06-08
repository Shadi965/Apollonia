#pragma once

#ifndef LYRICS_DAO_H
#define LYRICS_DAO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>

#include "repository_exceptions.h"
#include "entities.h"


// CREATE TABLE IF NOT EXISTS lyrics (
//     song_id INTEGER NOT NULL, 
//     time_ms INTEGER NOT NULL, 
//     line TEXT NOT NULL, 
//     UNIQUE (song_id, time_ms), 
//     FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE);
//
// CREATE INDEX IF NOT EXISTS idx_lyrics_song_time ON lyrics(song_id, time_ms);

// CREATE VIRTUAL TABLE IF NOT EXISTS lyrics_fts USING fts5(
//     song_id UNINDEXED,
//     line,
//     content='lyrics',
//     content_rowid='rowid'
// );

class LyricsDao {
public:
    LyricsDao(SQLite::Database& db);

    std::vector<LyricLineEntity> getLyricsForSong(int songId) const;

    bool addLyricLine(int songId, int timeMs, const std::string& line);

private:
    SQLite::Database& _db;
};

#endif // LYRICS_DAO_H
