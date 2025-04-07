#pragma once

#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <SQLiteCpp/SQLiteCpp.h>

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath);

    SQLite::Database& getDb();

private:
    SQLite::Database _db;


    void createSongs();
    void createSongsMeta();
    void createAlbums(const std::string& tableName);
    void createPlaylists(const std::string& tableName);
    void createLyrics();
    void createPlaylistSongs();

    void initDB();
    int checkVersion();

    void from_v1_to_v2();
};


#endif // DB_MANAGER_H
