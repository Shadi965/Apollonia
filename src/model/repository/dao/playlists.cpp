#include "playlists.h"
// CREATE TABLE IF NOT EXISTS playlists (
//     id INTEGER PRIMARY KEY, 
//     name TEXT);

std::vector<Playlist> PlaylistDao::getAllSongs() {
    std::vector<Playlist> playlists;
    SQLite::Statement query(_db, "SELECT * FROM playlists");

    while (query.executeStep()) {
        playlists.push_back({
            query.getColumn(0).getInt(),
            query.getColumn(1).getString()
        });
    }

    return playlists;
}

Playlist PlaylistDao::getPlaylistById(int playlistId) {
    SQLite::Statement query(_db, "SELECT * FROM playlists WHERE id = ?");
    query.bind(1, playlistId);

    // TODO: Реализовать нормальные исключения
    if (!query.executeStep()) 
        throw std::runtime_error("No playlist found with id: " + std::to_string(playlistId));

    return {
        query.getColumn(0).getInt(),
        query.getColumn(1).getString()
    };
}


int64_t PlaylistDao::insertPlaylist(const Playlist& playlist) {
    SQLite::Statement query(_db, "INSERT INTO playlists (name) VALUES (?)");
    query.bind(1, playlist.name);

    query.executeStep();

    return _db.getLastInsertRowid();
}

void PlaylistDao::updatePlaylistName(int playlistId, const std::string& newName) {
    SQLite::Statement query(_db, "UPDATE playlists SET name = ? WHERE id = ?");
    query.bind(1, newName);
    query.bind(2, playlistId);

    // TODO: Реализовать нормальные исключения
    if (!query.executeStep())
        throw std::runtime_error("No playlist found with id: " + std::to_string(playlistId));
}

void PlaylistDao::deletePlaylist(int playlistId) {
    SQLite::Statement query(_db, "DELETE FROM playlists WHERE id = ?");
    query.bind(1, playlistId);
    query.executeStep();
}
