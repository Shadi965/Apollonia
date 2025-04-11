#include "playlists.h"

PlaylistDao::PlaylistDao(SQLite::Database& db): _db(db) {};

std::vector<PlaylistEntity> PlaylistDao::getAllPlaylists() const {
    std::vector<PlaylistEntity> playlists;
    SQLite::Statement query(_db, "SELECT * FROM playlists");

    while (query.executeStep()) {
        playlists.push_back({
            query.getColumn(0).getInt(),
            query.getColumn(1).getString(),
            query.getColumn(2).getString()
        });
    }

    return playlists;
}
PlaylistEntity PlaylistDao::getPlaylistById(int id) const {
    SQLite::Statement query(_db, "SELECT * FROM playlists WHERE id = ?");
    query.bind(1, id);

    if (!query.executeStep()) 
        throw PlaylistNotFoundException(id);

    return {
        query.getColumn(0).getInt(),
        query.getColumn(1).getString(),
        query.getColumn(2).getString(),
    };
}

int PlaylistDao::insertPlaylist(const PlaylistEntity& playlist) {
    // TODO: Плейлисты могут выглядеть одинаково, но надо придумать защиту от случайных повторных запросов
    SQLite::Statement query(_db, "INSERT INTO playlists(name) VALUES (?)");
    query.bind(1, playlist.name);

    query.executeStep();

    return _db.getLastInsertRowid();
}
bool PlaylistDao::updatePlaylistName(int id, const std::string& newName) {
    SQLite::Statement query(_db, "UPDATE playlists SET name = ? WHERE id = ?");
    query.bind(1, newName);
    query.bind(2, id);

    query.executeStep();

    return _db.getChanges() != 0;
}
bool PlaylistDao::deletePlaylist(int id) {
    SQLite::Statement query(_db, "DELETE FROM playlists WHERE id = ?");
    query.bind(1, id);
    query.executeStep();

    return _db.getChanges() != 0;
}

bool PlaylistDao::updatePlaylistCoverPath(int id, const std::string& coverPath) {
    SQLite::Statement query(_db, "UPDATE playlists SET cover_path = ? WHERE id = ?");
    query.bind(1, coverPath);
    query.bind(2, id);

    query.executeStep();

    return _db.getChanges() != 0;
}