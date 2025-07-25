#include <sqlite3.h>

#include "albums.h"
AlbumDao::AlbumDao(SQLite::Database& db): _db(db) {};

std::vector<AlbumEntity> AlbumDao::getAllAlbums() const {
    std::vector<AlbumEntity> albums;
    SQLite::Statement query(_db, "SELECT * FROM albums");

    while (query.executeStep()) {
        albums.push_back({
            query.getColumn(0).getInt(),
            query.getColumn(1).getString(),
            query.getColumn(2).getString(),
            query.getColumn(3).getInt(),
            query.getColumn(4).getInt(),
            query.getColumn(5).getInt() != 0,
            query.getColumn(6).getString(),
            query.getColumn(7).getString(),
            query.getColumn(8).getString(),
            query.getColumn(9).getString()
        });
    }

    return albums;
}
AlbumEntity AlbumDao::getAlbumById(int albumId) const {
    SQLite::Statement query(_db, "SELECT * FROM albums WHERE id = ?");
    query.bind(1, albumId);

    if (!query.executeStep()) 
        throw AlbumNotFoundException(albumId);

    return {
        query.getColumn(0).getInt(),
        query.getColumn(1).getString(),
        query.getColumn(2).getString(),
        query.getColumn(3).getInt(),
        query.getColumn(4).getInt(),
        query.getColumn(5).getInt() != 0,
        query.getColumn(6).getString(),
        query.getColumn(7).getString(),
        query.getColumn(8).getString(),
        query.getColumn(9).getString()
    };
}
std::string AlbumDao::getAlbumTitleById(int albumId) const {
    SQLite::Statement query(_db, "SELECT title FROM albums WHERE id = ?");
    query.bind(1, albumId);

    if (!query.executeStep()) 
        throw AlbumNotFoundException(albumId);
    
    return query.getColumn(0).getString();
}
std::vector<int> AlbumDao::getAllAlbumsIds() const {
    std::vector<int> ids;

    SQLite::Statement query(_db, "SELECT id FROM albums");

    while (query.executeStep())
        ids.push_back(query.getColumn(0).getInt());

    return ids;
}
std::string AlbumDao::getAlbumCoverPathById(int id) const {
    SQLite::Statement query(_db, "SELECT cover_path FROM albums WHERE id = ?");
    query.bind(1, id);

    if (!query.executeStep()) 
        throw AlbumNotFoundException(id);

    return query.getColumn(0).getString();
}

int AlbumDao::checkAlbumExists(const std::string& artist, const std::string& title) const {
    SQLite::Statement query(_db, "SELECT id FROM albums WHERE title = ? AND artist = ?");
    query.bind(1, title);
    query.bind(2, artist);
    
    return query.executeStep() ? query.getColumn(0).getInt() : 0;
}

int AlbumDao::insertAlbum(const AlbumEntity& album) {
    SQLite::Statement query(_db, 
        "INSERT INTO albums (title, artist, track_count, disc_count, compilation, date, copyright, genre, cover_path) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );
    query.bind(1, album.title);
    query.bind(2, album.artist);
    query.bind(3, album.track_count);
    query.bind(4, album.disc_count);
    query.bind(5, album.compilation);
    query.bind(6, album.date);
    query.bind(7, album.copyright);
    query.bind(8, album.genre);
    query.bind(9, album.cover_path);

    try {
        query.executeStep();
    }
    catch(const SQLite::Exception& e) {
        if (e.getExtendedErrorCode() == SQLITE_CONSTRAINT_UNIQUE)
            return 0;
        throw e;
    }
    

    return _db.getLastInsertRowid();
}
bool AlbumDao::updateAlbumDateAndGenre(int albumId, const std::string& newDate, const std::string& newGenre) {
    SQLite::Statement query(_db, "UPDATE albums SET date = ?, genre = ? WHERE id = ?");
    query.bind(1, newDate);
    query.bind(2, newGenre);
    query.bind(3, albumId);

    query.executeStep();
    return _db.getChanges() != 0;
}
bool AlbumDao::updateAlbumCoverPath(int id, const std::string& coverPath) {
    SQLite::Statement query(_db, "UPDATE albums SET cover_path = ? WHERE id = ?");
    query.bind(1, coverPath);
    query.bind(2, id);

    query.executeStep();

    return _db.getChanges() != 0;
}
bool AlbumDao::updateAlbumTrackCount(int id, int trackCount) {
    SQLite::Statement query(_db, "UPDATE albums SET track_count = ? WHERE id = ?");
    query.bind(1, trackCount);
    query.bind(2, id);

    query.executeStep();

    return _db.getChanges() != 0;
}

std::vector<int> AlbumDao::getSongsFromAlbum(int albumId) const {
    std::vector<int> songIds;
    SQLite::Statement query(_db, "SELECT id FROM songs WHERE album_id = ?");
    query.bind(1, albumId);

    while (query.executeStep())  {
        songIds.push_back(query.getColumn(0).getInt());
    }

    return songIds;
}
    
