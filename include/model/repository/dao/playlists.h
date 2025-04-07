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
    PlaylistEntity getPlaylistById(int playlistId) const;

    int insertPlaylist(const PlaylistEntity& playlist);
    bool updatePlaylistName(int playlistId, const std::string& newName);
    bool deletePlaylist(int playlistId);

private:
    SQLite::Database& _db;
};

#endif // PLAYLIST_DAO_H
