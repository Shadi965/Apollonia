#include "playlist_songs.h"

PlaylistSongDao::PlaylistSongDao(SQLite::Database& db) : _db(db) {}

std::vector<int> PlaylistSongDao::getSongsInPlaylist(int playlistId) const {
    std::vector<int> songIds;
    SQLite::Statement query(_db, "SELECT song_id FROM playlist_songs WHERE playlist_id = ?");
    query.bind(1, playlistId);

    while (query.executeStep()) {
        songIds.push_back(query.getColumn(0).getInt());
    }

    return songIds;
}

bool PlaylistSongDao::addSongToPlaylist(int playlistId, int songId) {
    // TODO: Возможно исключение при несуществующих playlist_id или song_id
    SQLite::Statement query(_db, "INSERT INTO playlist_songs (playlist_id, song_id) VALUES (?, ?)");
    query.bind(1, playlistId);
    query.bind(2, songId);
    query.executeStep();
    return _db.getChanges() != 0;
}
bool PlaylistSongDao::removeSongFromPlaylist(int playlistId, int songId) {
    SQLite::Statement query(_db, "DELETE FROM playlist_songs WHERE playlist_id = ? AND song_id = ?");
    query.bind(1, playlistId);
    query.bind(2, songId);
    query.executeStep();

    return _db.getChanges() != 0;
}
