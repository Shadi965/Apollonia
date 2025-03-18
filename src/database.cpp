#include "database.h"

#define CREATE_TABLE_IF_NOT_EXISTS "CREATE TABLE IF NOT EXISTS songs (id INTEGER PRIMARY KEY, title TEXT, artist TEXT, file TEXT, duration INTEGER)"
#define GET_ALL_SONGS "SELECT id, title, artist, file, duration FROM songs"
#define GET_SONG_BY_ID "SELECT id, title, artist, file, duration FROM songs WHERE id = ?"

Database::Database(const std::string& dbPath) : db(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    SQLite::Database db("songs.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    db.exec(CREATE_TABLE_IF_NOT_EXISTS);
}

std::vector<Song> Database::getAllSongs() {
    std::vector<Song> songs;
    SQLite::Statement query(db, GET_ALL_SONGS);

    while(query.executeStep()) {
        songs.push_back({
            query.getColumn(0).getInt(),
            query.getColumn(1).getString(),
            query.getColumn(2).getString(),
            query.getColumn(3).getString(),
            query.getColumn(4).getInt()
        });
    }
    return songs;
}

Song Database::getSongById(int id) {
    SQLite::Statement query(db, GET_SONG_BY_ID);
    query.bind(1, id);
    if (query.executeStep()) {
        return {
            query.getColumn(0).getInt(),
            query.getColumn(1).getString(),
            query.getColumn(2).getString(),
            query.getColumn(3).getString(),
            query.getColumn(4).getInt()
        };
    }
    return {-1, "", "", "", -1};
    
}