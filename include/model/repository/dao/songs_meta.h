#pragma once

#ifndef SONG_META_DAO_H
#define SONG_META_DAO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>

#include "repository_exceptions.h"
#include "entities.h"


// CREATE TABLE IF NOT EXISTS songs_meta (
//     song_id INTEGER, 
//     duration INTEGER, 
//     bitrate INTEGER, 
//     channels INTEGER, 
//     sample_rate INTEGER, 
//     last_modified INTEGER NOT NULL, 
//     file TEXT UNIQUE, 
//     FOREIGN KEY (song_id) REFERENCES songs(id) ON DELETE CASCADE);


class SongMetaDao {
public:
    SongMetaDao(SQLite::Database& db);

    std::vector<SongMetaEntity> getAllSongsData() const;
    SongMetaEntity getSongDataById(int songId) const;
    int getSongDuration(int songId) const;
    std::string getFilePathBySongId(int songId) const;

    bool insertSongData(const SongMetaEntity& meta);

private:
    SQLite::Database& _db;
};

#endif // SONG_META_DAO_H
