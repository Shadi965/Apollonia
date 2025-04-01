#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <SQLiteCpp/SQLiteCpp.h>

class DatabaseManager {
private:
    SQLite::Database db;

    void createSongInfo();

    void createSongMeta();

    void createAlbums();

    void createPlaylists();

    void createLyrics();

    void createPlaylistSongs();

    void initDB();

public:
    DatabaseManager(const std::string& dbPath) : db(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
        initDB();
    }

    SQLite::Database& getDb() {
        return db;
    }
};


#endif // DB_MANAGER_H
