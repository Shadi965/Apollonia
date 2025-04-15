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


    void createSongs(const std::string& tableName);
    void createSongsMeta(const std::string& tableName);
    void createAlbums(const std::string& tableName);
    void createPlaylists(const std::string& tableName);
    void createLyrics(const std::string& tableName);
    void createPlaylistSongs(const std::string& tableName);

    void initDB();
    int checkVersion();

    void v1_to_v2();
    void v2_to_v3();
};


#endif // DB_MANAGER_H
