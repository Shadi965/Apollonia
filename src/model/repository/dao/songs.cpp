#include <sqlite3.h>

#include "songs.h"

SongDao::SongDao(SQLite::Database& db): _db(db) {};

std::vector<SongEntity> SongDao::getAllSongs() const {
    std::vector<SongEntity> songs;
    SQLite::Statement query(_db, "SELECT * FROM songs");

    while (query.executeStep()) {
        songs.push_back({
            query.getColumn(0).getInt(),
            query.getColumn(1).getString(),
            query.getColumn(2).getString(),
            query.getColumn(3).getString(),
            query.getColumn(4).getInt(),
            query.getColumn(5).getInt(),
            query.getColumn(6).getInt(),
            query.getColumn(7).getString(),
            query.getColumn(8).getString(),
            query.getColumn(9).getString()
        });
    }

    return songs;
}
SongEntity SongDao::getSongById(int songId) const {
    SQLite::Statement query(_db, 
        "SELECT * FROM songs WHERE id = ?"
    );
    query.bind(1, songId);

    
    if (!query.executeStep()) 
        throw SongNotFoundException(songId);


    return {
        query.getColumn(0).getInt(),
        query.getColumn(1).getString(),
        query.getColumn(2).getString(),
        query.getColumn(3).getString(),
        query.getColumn(4).getInt(),
        query.getColumn(5).getInt(),
        query.getColumn(6).getInt(),
        query.getColumn(7).getString(),
        query.getColumn(8).getString(),
        query.getColumn(9).getString()
    };
}

int SongDao::insertSong(const SongEntity& song) {
    SQLite::Statement query(_db, 
        "INSERT INTO songs (title, artist, composer, album_id, track, disc, date, copyright, genre) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.bind(1, song.title);
    query.bind(2, song.artist);
    query.bind(3, song.composer);
    
    song.album_id == 0 ? query.bind(4) : query.bind(4, song.album_id);

    query.bind(5, song.track);
    query.bind(6, song.disc);
    query.bind(7, song.date);
    query.bind(8, song.copyright);
    query.bind(9, song.genre);

    try {
        query.executeStep();
    }
    catch(const SQLite::Exception& e) {
        if (e.getExtendedErrorCode() == SQLITE_CONSTRAINT_FOREIGNKEY)
            throw AlbumNotFoundException(song.album_id);
        if (e.getExtendedErrorCode() == SQLITE_CONSTRAINT_UNIQUE)
            throw SongAlreadyExists();
        throw e;
    }
    
    return _db.getLastInsertRowid();
}
bool SongDao::deleteSongById(int songId) {
    SQLite::Statement query(_db, "DELETE FROM songs WHERE id = ?");
    query.bind(1, songId);
    query.executeStep();
    
    return _db.getChanges() != 0;
}

std::string SongDao::getNewestSongDateInAlbum(int albumId) const {
    SQLite::Statement query(_db, "SELECT MAX(date) FROM songs WHERE album_id = ?");
    query.bind(1, albumId);

    if (!query.executeStep())
        return "";

    return query.getColumn(0).getString();
}
std::string SongDao::getMostFrequentGenreInAlbum(int albumId) const {
    SQLite::Statement query(_db, "SELECT genre, COUNT(genre) as count "
                                "FROM songs WHERE album_id = ? "
                                "GROUP BY genre ORDER BY count DESC LIMIT 1");
    query.bind(1, albumId);

    if (!query.executeStep())
        return "";

    return query.getColumn(0).getString();
}
std::vector<SongEntity> SongDao::getTitleArtistAlbumTuples() const {
    std::vector<SongEntity> tuples;

    SQLite::Statement query(_db, "SELECT id, title, artist, album_id FROM songs");

    SongEntity song;

    while (query.executeStep()) {
        song.id = query.getColumn(0).getInt();
        song.title = query.getColumn(1).getString();
        song.artist = query.getColumn(2).getString();
        song.album_id = query.getColumn(3).getInt();

        tuples.push_back(song);
    }

    return tuples;
}
std::vector<int> SongDao::getSongsFromAlbum(int albumId) const {
    std::vector<int> songIds;

    SQLite::Statement query(_db, "SELECT id FROM songs WHERE album_id = ? ORDER BY disc ASC, track ASC");

    query.bind(1, albumId);

    while (query.executeStep())
        songIds.push_back(query.getColumn(0).getInt());

    return songIds;
}

static std::string ftsQuery(const std::string& query) {
    std::istringstream iss{std::string(query)};
    std::string token;
    std::string result;

    while (iss >> token) {
        if (!result.empty()) {
            result += ' ';
        }
        result += token + '*';
    }

    return result;
}

std::vector<int> SongDao::searchSongs(const std::string& query) const {
    std::vector<int> songIds;

    SQLite::Statement stmt(_db, "SELECT id FROM ("
                                    "SELECT songs.id AS id, 2 AS score, bm25(songs_fts, 0.5, 0.8) AS rank "
                                    "FROM songs_fts "
                                    "JOIN songs ON songs_fts.rowid = songs.id "
                                    "WHERE songs_fts MATCH ? "

                                    "UNION "

                                    "SELECT song_id AS id, 1 AS score, NULL AS rank "
                                    "FROM lyrics_fts "
                                    "WHERE lyrics_fts MATCH ? "
                                ") "
                                "GROUP BY id "
                                "ORDER BY MAX(score) DESC, MIN(rank) ASC NULLS LAST"
                            );
    stmt.bind(1, ftsQuery(query));
    stmt.bind(2, ftsQuery(query));

    while (stmt.executeStep())
        songIds.push_back(stmt.getColumn(0).getInt());

    return songIds;
}
