#ifndef PLAYLIST_SONG_DAO_H
#define PLAYLIST_SONG_DAO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include <stdexcept>

// CREATE TABLE IF NOT EXISTS playlist_songs (
//     playlist_id INTEGER NOT NULL, 
//     song_id INTEGER NOT NULL, 
//     PRIMARY KEY(playlist_id, song_id), 
//     FOREIGN KEY(playlist_id) REFERENCES playlists(id) ON DELETE CASCADE, 
//     FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE);

struct PlaylistSong {
    int playlistId;
    int songId;
};

class PlaylistSongDao {
public:
    PlaylistSongDao(SQLite::Database& db) : _db(db) {}

    void addSongToPlaylist(int playlistId, int songId);

    void removeSongFromPlaylist(int playlistId, int songId);

    std::vector<int> getSongsInPlaylist(int playlistId);

private:
    SQLite::Database& _db;
};

#endif // PLAYLIST_SONG_DAO_H
