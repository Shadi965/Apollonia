#pragma once

#ifndef ALBUM_DAO_H
#define ALBUM_DAO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>

#include "repository_exceptions.h"
#include "entities.h"


// CREATE TABLE IF NOT EXISTS albums (
//     id INTEGER PRIMARY KEY AUTOINCREMENT, 
//     title TEXT, 
//     artist TEXT, 
//     track_count INTEGER, 
//     disc_count INTEGER, 
//     compilation BOOLEAN DEFAULT FALSE, 
//     date TEXT, 
//     copyright TEXT, 
//     genre TEXT, 
//     cover_path TEXT, 
//     UNIQUE (title, artist));


class AlbumDao {
public:
    AlbumDao(SQLite::Database& db);

    std::vector<AlbumEntity> getAllAlbums() const;
    AlbumEntity getAlbumById(int albumId) const;
    std::string getAlbumTitleById(int albumId) const;
    std::vector<int> getAllAlbumsIds() const;
    std::string getAlbumCoverPathById(int id) const;

    int checkAlbumExists(const std::string& artist, const std::string& title) const;

    int insertAlbum(const AlbumEntity& album);
    bool updateAlbumDateAndGenre(int albumId, const std::string& newDate, const std::string& newGenre);
    bool updateAlbumCoverPath(int id, const std::string& coverPath);

private:
    SQLite::Database& _db;
};

#endif // ALBUM_DAO_H
