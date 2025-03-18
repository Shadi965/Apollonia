#include "database.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <filesystem>
#include <iostream>

#define CREATE_TABLE_IF_NOT_EXISTS "CREATE TABLE IF NOT EXISTS songs (id INTEGER PRIMARY KEY, title TEXT, artist TEXT, file TEXT, duration INTEGER)"
#define GET_ALL_SONGS "SELECT id, title, artist, file, duration FROM songs"
#define GET_SONG_BY_ID "SELECT id, title, artist, file, duration FROM songs WHERE id = ?"

#define DELETE_ALL_SONGS "DELETE FROM songs"
#define INSERT_SONG "INSERT INTO songs (title, artist, file, duration) VALUES (?, ?, ?, ?)"

Database::Database(const std::string& dbPath) : db(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
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

void Database::updateDatabase(const std::string& musicFolder) {
    db.exec(DELETE_ALL_SONGS);
    SQLite::Statement insert(db, INSERT_SONG);

    for (const auto& entry : std::filesystem::directory_iterator(musicFolder)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();
            TagLib::FileRef fileRef(filePath.c_str());
            if (!fileRef.isNull() && fileRef.tag()) {
                TagLib::Tag* tag = fileRef.tag();
                std::string title = tag->title().to8Bit(true);
                std::string artist = tag->artist().to8Bit(true);
                int duration = fileRef.audioProperties() ? fileRef.audioProperties()->length() : 0;

                insert.bind(1, title);
                insert.bind(2, artist);
                insert.bind(3, filePath);
                insert.bind(4, duration);
                insert.exec();
                insert.reset();
            }
        }
        std::cout << "Database updated based on folder content." << std::endl;
    }
    
}