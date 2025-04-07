#pragma once

#ifndef SONG_DAO_H
#define SONG_DAO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>

#include "repository_exceptions.h"
#include "entities.h"

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
//
// CREATE INDEX IF NOT EXISTS idx_songs_album_id ON songs(album_id);

class SongDao {
public:
    SongDao(SQLite::Database& db);
    
    std::vector<SongEntity> getAllSongs() const;
    SongEntity getSongById(int songId) const;

    std::string getNewestSongDateInAlbum(int albumId) const;
    std::string getMostFrequentGenreInAlbum(int albumId) const;
    std::vector<SongEntity> getTitleArtistAlbumTuples() const;

    int insertSong(const SongEntity& song);
    bool deleteSongById(int songId);

private:
    SQLite::Database& _db;
};

#endif // SONG_DAO_H
