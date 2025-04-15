#include <sqlite3.h>

#include "songs_meta.h"

SongMetaDao::SongMetaDao(SQLite::Database& db): _db(db) {};

std::vector<SongMetaEntity> SongMetaDao::getAllSongsData() const {
    std::vector<SongMetaEntity> songsMeta;
    SQLite::Statement query(_db, "SELECT * FROM songs_meta");

    while (query.executeStep()) {
        songsMeta.push_back({
            query.getColumn(0).getInt(),
            query.getColumn(1).getInt(),
            query.getColumn(2).getInt(),
            query.getColumn(3).getInt(),
            query.getColumn(4).getInt(),
            query.getColumn(5).getInt(),
            query.getColumn(6).getString()
        });
    }

    return songsMeta;
}
SongMetaEntity SongMetaDao::getSongDataById(int songId) const {
    SQLite::Statement query(_db, "SELECT * FROM songs_meta WHERE song_id = ?");
    query.bind(1, songId);

    if (!query.executeStep()) 
        throw SongNotFoundException(songId);

    return {
        query.getColumn(0).getInt(),
        query.getColumn(1).getInt(),
        query.getColumn(2).getInt(),
        query.getColumn(3).getInt(),
        query.getColumn(4).getInt(),
        query.getColumn(5).getInt(),
        query.getColumn(6).getString()
    };
}
int SongMetaDao::getSongDuration(int songId) const {
    SQLite::Statement query(_db, "SELECT duration FROM songs_meta WHERE song_id = ?");
    query.bind(1, songId);

    if (!query.executeStep()) 
        throw SongNotFoundException(songId);

    return query.getColumn(0).getInt();
}
std::string SongMetaDao::getFilePathBySongId(int songId) const {
    SQLite::Statement query(_db, "SELECT file FROM songs_meta WHERE song_id = ?");
    query.bind(1, songId);
    
    if (!query.executeStep())
        throw SongNotFoundException(songId);
    
    return query.getColumn(0).getString();
}


bool SongMetaDao::insertSongData(const SongMetaEntity& meta) {
    SQLite::Statement query(_db, 
        "INSERT INTO songs_meta (song_id, duration, bitrate, channels, sample_rate, last_modified, file) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);"
    );
    query.bind(1, meta.song_id);
    query.bind(2, meta.duration);
    query.bind(3, meta.bitrate);
    query.bind(4, meta.channels);
    query.bind(5, meta.sample_rate);
    query.bind(6, meta.last_modified);
    query.bind(7, meta.file);

    try {
        query.executeStep();
    }
    catch(const SQLite::Exception& e) {
        if (e.getExtendedErrorCode() == SQLITE_CONSTRAINT_UNIQUE || 
            e.getExtendedErrorCode() == SQLITE_CONSTRAINT_FOREIGNKEY)
            return 0;
        throw e;
    }
    
    return _db.getChanges() != 0;
}
