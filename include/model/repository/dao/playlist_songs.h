#pragma once

#ifndef PLAYLIST_SONG_DAO_H
#define PLAYLIST_SONG_DAO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>

#include "repository_exceptions.h"
#include "entities.h"


// CREATE TABLE IF NOT EXISTS playlist_songs (
//     playlist_id INTEGER NOT NULL, 
//     song_id INTEGER NOT NULL, 
//     position REAL, 
//     PRIMARY KEY(playlist_id, song_id), 
//     UNIQUE(playlist_id, position),
//     FOREIGN KEY(playlist_id) REFERENCES playlists(id) ON DELETE CASCADE, 
//     FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE);


class PlaylistSongDao {
public:
    PlaylistSongDao(SQLite::Database& db);

    std::vector<std::pair<int, double>> getSongsFromPlaylist(int playlistId) const;

    bool addSongToPlaylist(int playlistId, int songId, double position);
    bool removeSongFromPlaylist(int playlistId, int songId);

    bool updateSongPosition(int playlistId, int songId, double position);

    void spreadOutPositions(int playlistId, double startPosition = 1.0, double step = 10.0);
private:
    SQLite::Database& _db;
};

#endif // PLAYLIST_SONG_DAO_H
