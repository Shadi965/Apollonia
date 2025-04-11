#pragma once

#ifndef PLAYLIST_DAO_H
#define PLAYLIST_DAO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>

#include "repository_exceptions.h"
#include "entities.h"


// CREATE TABLE IF NOT EXISTS playlists (
//     id INTEGER PRIMARY KEY, 
//     name TEXT, 
//     cover_path TEXT);


class PlaylistDao {
public:
    PlaylistDao(SQLite::Database& db);

    std::vector<PlaylistEntity> getAllPlaylists() const;
    PlaylistEntity getPlaylistById(int id) const;

    int insertPlaylist(const PlaylistEntity& playlist);
    bool updatePlaylistName(int id, const std::string& newName);
    bool deletePlaylist(int id);

    bool updatePlaylistCoverPath(int id, const std::string& coverPath);

private:
    SQLite::Database& _db;
};

#endif // PLAYLIST_DAO_H
