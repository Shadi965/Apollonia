#ifndef PLAYLIST_DAO_H
#define PLAYLIST_DAO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <stdexcept>
#include <vector>

// CREATE TABLE IF NOT EXISTS playlists (
//     id INTEGER PRIMARY KEY, 
//     name TEXT);

struct Playlist {
    int id;
    std::string name;
};

class PlaylistDao {
public:
    PlaylistDao(SQLite::Database& db): _db(db) {};

    std::vector<Playlist> getAllPlaylists();

    Playlist getPlaylistById(int playlistId);


    int64_t insertPlaylist(const Playlist& playlist);

    int updatePlaylistName(int playlistId, const std::string& newName);

    void deletePlaylist(int playlistId);

private:
    SQLite::Database& _db;
};

#endif // PLAYLIST_DAO_H
