#include "songs_meta.h"

// CREATE TABLE IF NOT EXISTS songs_meta (
//     song_id INTEGER, 
//     duration INTEGER, 
//     bitrate INTEGER, 
//     channels INTEGER, 
//     sample_rate INTEGER, 
//     last_modified INTEGER NOT NULL, 
//     file TEXT UNIQUE, 
//     FOREIGN KEY (song_id) REFERENCES songs(id) ON DELETE CASCADE);

std::vector<SongMeta> SongMetaDao::getAllSongsData() {
    std::vector<SongMeta> songsMeta;
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

SongMeta SongMetaDao::getSongDataById(int songId) {
    SQLite::Statement query(_db, "SELECT song_id, duration, bitrate, channels, sample_rate, last_modified, file "
                                "FROM songs_meta WHERE song_id = ?");
    query.bind(1, songId);

    // TODO: Реализовать нормальные исключения
    if (!query.executeStep()) 
        throw std::runtime_error("No song metadata found with id: " + std::to_string(songId));

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

int SongMetaDao::getSongDuration(int songId) {
    SQLite::Statement query(_db, "SELECT duration FROM songs_meta WHERE song_id = ?");
    query.bind(1, songId);

    // TODO: Реализовать нормальные исключения
    if (!query.executeStep()) 
        throw std::runtime_error("No song metadata found with id: " + std::to_string(songId));

    return query.getColumn(0).getInt();
}


int64_t SongMetaDao::insertSongData(const SongMeta& meta) {
    // TODO: Возможна ошибка из-за передачи несуществующего song_id
    SQLite::Statement query(_db, 
        "INSERT INTO songs_meta (song_id, duration, bitrate, channels, sample_rate, last_modified, file) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);"
    );
    query.bind(1, meta.songId);
    query.bind(2, meta.duration);
    query.bind(3, meta.bitrate);
    query.bind(4, meta.channels);
    query.bind(5, meta.sampleRate);
    query.bind(6, meta.lastModified);
    query.bind(7, meta.file);

    query.executeStep();

    return _db.getLastInsertRowid();
}

std::string SongMetaDao::getFilePathBySongId(int songId) {
    SQLite::Statement query(_db, "SELECT file FROM songs_meta WHERE song_id = ?");
    query.bind(1, songId);
    
    // TODO: Реализовать нормальные исключения
    if (!query.executeStep())
        throw std::runtime_error("File path not found for song_id: " + std::to_string(songId));
    
    return query.getColumn(0).getString();
}
