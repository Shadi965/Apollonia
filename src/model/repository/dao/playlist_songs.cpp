#include <sqlite3.h>

#include "playlist_songs.h"

PlaylistSongDao::PlaylistSongDao(SQLite::Database& db) : _db(db) {}

std::vector<std::pair<int, double>> PlaylistSongDao::getSongsFromPlaylist(int playlistId) const {
    std::vector<std::pair<int, double>> songs;
    SQLite::Statement query(_db, "SELECT song_id, position FROM playlist_songs WHERE playlist_id = ? ORDER BY position");
    query.bind(1, playlistId);

    while (query.executeStep()) {
        songs.push_back({
            query.getColumn(0).getInt(), 
            query.getColumn(1).getDouble()
        });
    }

    return songs;
}

bool PlaylistSongDao::addSongToPlaylist(int playlistId, int songId, double position) {
    SQLite::Statement query(_db, "INSERT INTO playlist_songs (playlist_id, song_id, position) VALUES (?, ?, ?)");
    query.bind(1, playlistId);
    query.bind(2, songId);
    query.bind(3, position);

    try {
        query.executeStep();
    }
    catch(const SQLite::Exception& e) {
        if (e.getExtendedErrorCode() == SQLITE_CONSTRAINT_PRIMARYKEY || 
            e.getExtendedErrorCode() == SQLITE_CONSTRAINT_FOREIGNKEY ||
            e.getExtendedErrorCode() == SQLITE_CONSTRAINT_UNIQUE)
            return false;
        throw e;
    }

    return _db.getChanges() != 0;
}
bool PlaylistSongDao::removeSongFromPlaylist(int playlistId, int songId) {
    SQLite::Statement query(_db, "DELETE FROM playlist_songs WHERE playlist_id = ? AND song_id = ?");
    query.bind(1, playlistId);
    query.bind(2, songId);
    query.executeStep();

    return _db.getChanges() != 0;
}

bool PlaylistSongDao::updateSongPosition(int playlistId, int songId, double position) {
    SQLite::Statement query(_db, "UPDATE playlist_songs SET position = ? WHERE playlist_id = ? AND song_id = ?");
    query.bind(1, position);
    query.bind(2, playlistId);
    query.bind(3, songId);
    try {
        query.executeStep();
    }
    catch(const SQLite::Exception& e) {
        if (e.getExtendedErrorCode() == SQLITE_CONSTRAINT_PRIMARYKEY || 
            e.getExtendedErrorCode() == SQLITE_CONSTRAINT_FOREIGNKEY ||
            e.getExtendedErrorCode() == SQLITE_CONSTRAINT_UNIQUE)
            return false;
        throw e;
    }

    return _db.getChanges() != 0;
}

void PlaylistSongDao::spreadOutPositions(int playlistId, double startPosition, double step) {
    SQLite::Transaction transaction(_db);

    std::vector<int> songIds;

    SQLite::Statement songs(_db, "SELECT song_id FROM playlist_songs WHERE playlist_id = ? ORDER BY position");
    songs.bind(1, playlistId);

    while (songs.executeStep()) {
        songIds.push_back(songs.getColumn(0).getInt());
    }

    SQLite::Statement spread(_db, "UPDATE playlist_songs SET position = ? WHERE playlist_id = ? AND song_id = ?");
    for (int songId : songIds) {
        spread.bind(1, startPosition);
        spread.bind(2, playlistId);
        spread.bind(3, songId);
        spread.exec();
        spread.reset();
        startPosition += step;
    }

    transaction.commit();
}