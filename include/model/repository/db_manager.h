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
    void createSongsFts(const std::string& tableName);
    void createSongsMeta(const std::string& tableName);
    void createAlbums(const std::string& tableName);
    void createPlaylists(const std::string& tableName);
    void createLyrics(const std::string& tableName);
    void createLyricsFts(const std::string& tableName);
    void createPlaylistSongs(const std::string& tableName);

    void rebuildFts();

    void initDB();
    int getDbVersion();
};


#endif // DB_MANAGER_H
