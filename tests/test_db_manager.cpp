#include <gtest/gtest.h>
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>

#include "db_manager.h"

#define DB_NAME "test_db_manager.db"

class DatabaseManagerTest : public ::testing::Test {
protected:
    std::string testDbPath = DB_NAME;

    void TearDown() override {
        if (std::filesystem::exists(testDbPath))
            std::filesystem::remove(testDbPath);
    }

    
    static bool hasColumn(SQLite::Database& db, const std::string& table, const std::string& column) {
        SQLite::Statement query(db, "PRAGMA table_info(" + table + ");");
        while (query.executeStep()) {
            if (query.getColumn(1).getText() == column) {
                return true;
            }
        }
        return false;
    }

    static SQLite::Database createV1(const std::string& dbPath) {
        SQLite::Database db(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        db.exec("CREATE TABLE metadata (key TEXT PRIMARY KEY, value TEXT);");
        db.exec("INSERT INTO metadata (key, value) VALUES ('db_version', '1');");

        db.exec("CREATE TABLE IF NOT EXISTS albums ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "title TEXT, "
            "artist TEXT, "
            "track_count INTEGER, "
            "disc_count INTEGER, "
            "compilation INTEGER, "
            "date TEXT, "
            "copyright TEXT, "
            "genre TEXT, "
            "UNIQUE (title, artist));"
        );
        
        db.exec("CREATE TABLE IF NOT EXISTS songs ("
            "id INTEGER PRIMARY KEY, "
            "title TEXT, "
            "artist TEXT, "
            "composer TEXT, "
            "album_id INTEGER, "
            "track INTEGER, "
            "disc INTEGER, "
            "date TEXT, "
            "copyright TEXT, "
            "genre TEXT, "
            "FOREIGN KEY (album_id) REFERENCES albums(id) ON DELETE SET NULL);"
    
            "CREATE INDEX IF NOT EXISTS idx_songs_album_id ON songs(album_id);"
        );

        db.exec("CREATE TABLE IF NOT EXISTS songs_meta ("
            "song_id INTEGER, "
            "duration INTEGER, "
            "bitrate INTEGER, "
            "channels INTEGER, "
            "sample_rate INTEGER, "
            "last_modified INTEGER NOT NULL, "
            "file TEXT UNIQUE, "
            "FOREIGN KEY (song_id) REFERENCES songs(id) ON DELETE CASCADE);"
        );

        db.exec("CREATE TABLE IF NOT EXISTS playlists ("
            "id INTEGER PRIMARY KEY, "
            "name TEXT);"
        );

        db.exec("CREATE TABLE IF NOT EXISTS lyrics ("
            "song_id INTEGER NOT NULL, "
            "time_ms INTEGER NOT NULL, "
            "line TEXT NOT NULL, "
            "FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE); "
            
            "CREATE INDEX IF NOT EXISTS idx_lyrics_song_time ON lyrics(song_id, time_ms);"
        );

        db.exec("CREATE TABLE IF NOT EXISTS playlist_songs ("
            "playlist_id INTEGER NOT NULL, "
            "song_id INTEGER NOT NULL, "
            "PRIMARY KEY(playlist_id, song_id), "
            "FOREIGN KEY(playlist_id) REFERENCES playlists(id) ON DELETE CASCADE, "
            "FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE);"
        );

        return db;
    }

    static SQLite::Database createV2(const std::string& dbPath) {
        SQLite::Database db(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        db.exec("CREATE TABLE metadata (key TEXT PRIMARY KEY, value TEXT);");
        db.exec("INSERT INTO metadata (key, value) VALUES ('db_version', '2');");

        db.exec("CREATE TABLE IF NOT EXISTS songs ("
            "id INTEGER PRIMARY KEY, "
            "title TEXT, "
            "artist TEXT, "
            "composer TEXT, "
            "album_id INTEGER, "
            "track INTEGER, "
            "disc INTEGER, "
            "date TEXT, "
            "copyright TEXT, "
            "genre TEXT, "
            "FOREIGN KEY (album_id) REFERENCES albums(id) ON DELETE SET NULL);"
    
            "CREATE INDEX IF NOT EXISTS idx_songs_album_id ON songs(album_id);"
        );

        db.exec("CREATE TABLE IF NOT EXISTS songs_meta ("
            "song_id INTEGER, "
            "duration INTEGER, "
            "bitrate INTEGER, "
            "channels INTEGER, "
            "sample_rate INTEGER, "
            "last_modified INTEGER NOT NULL, "
            "file TEXT UNIQUE, "
            "FOREIGN KEY (song_id) REFERENCES songs(id) ON DELETE CASCADE);"
        );

        db.exec("CREATE TABLE IF NOT EXISTS albums ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "title TEXT, "
            "artist TEXT, "
            "track_count INTEGER, "
            "disc_count INTEGER, "
            "compilation BOOLEAN DEFAULT FALSE, "
            "date TEXT, "
            "copyright TEXT, "
            "genre TEXT, "
            "cover_path TEXT, "
            "UNIQUE (title, artist));"
        );

        db.exec("CREATE TABLE IF NOT EXISTS playlists ("
            "id INTEGER PRIMARY KEY, "
            "name TEXT, "
            "cover_path TEXT);"
        );

        db.exec("CREATE TABLE IF NOT EXISTS lyrics ("
            "song_id INTEGER NOT NULL, "
            "time_ms INTEGER NOT NULL, "
            "line TEXT NOT NULL, "
            "FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE); "
            
            "CREATE INDEX IF NOT EXISTS idx_lyrics_song_time ON lyrics(song_id, time_ms);"
        );

        db.exec("CREATE TABLE IF NOT EXISTS playlist_songs ("
            "playlist_id INTEGER NOT NULL, "
            "song_id INTEGER NOT NULL, "
            "PRIMARY KEY(playlist_id, song_id), "
            "FOREIGN KEY(playlist_id) REFERENCES playlists(id) ON DELETE CASCADE, "
            "FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE);"
        );

        return db;
    }
};

TEST_F(DatabaseManagerTest, DBIsCreatedAndInitialized) {
    DatabaseManager dbManager(testDbPath);
    SQLite::Database& db = dbManager.getDb();

    std::vector<std::string> tables = {
        "songs", "songs_meta", "albums", "playlists",
        "lyrics", "playlist_songs", "metadata"
    };

    for (const auto& table : tables)
        ASSERT_TRUE(db.tableExists(table));

    SQLite::Statement query(db, "SELECT value FROM metadata WHERE key = 'db_version';");
    ASSERT_TRUE(query.executeStep());
    EXPECT_EQ(query.getColumn(0).getText(), std::string("3"));
}

TEST_F(DatabaseManagerTest, V1ToV3Upgrade) {
    SQLite::Database db = createV1(testDbPath);

    DatabaseManager dbManager(testDbPath);
    SQLite::Database& newDb = dbManager.getDb();

    ASSERT_TRUE(newDb.tableExists("albums"));
    ASSERT_TRUE(newDb.tableExists("playlist_songs"));
    ASSERT_TRUE(newDb.tableExists("lyrics"));
    ASSERT_TRUE(newDb.tableExists("songs_meta"));
    ASSERT_TRUE(newDb.tableExists("songs"));
    ASSERT_TRUE(newDb.tableExists("playlists"));

    SQLite::Statement versionQuery(newDb, "SELECT value FROM metadata WHERE key = 'db_version';");
    ASSERT_TRUE(versionQuery.executeStep());
    EXPECT_EQ(versionQuery.getColumn(0).getText(), std::string("3"));

    ASSERT_TRUE(hasColumn(newDb, "albums", "cover_path"));
    ASSERT_TRUE(hasColumn(newDb, "playlists", "cover_path"));
}

TEST_F(DatabaseManagerTest, V2ToV3Upgrade) {
    SQLite::Database db = createV2(testDbPath);

    DatabaseManager dbManager(testDbPath);
    SQLite::Database& newDb = dbManager.getDb();

    ASSERT_TRUE(newDb.tableExists("albums"));
    ASSERT_TRUE(newDb.tableExists("playlist_songs"));
    ASSERT_TRUE(newDb.tableExists("lyrics"));
    ASSERT_TRUE(newDb.tableExists("songs_meta"));
    ASSERT_TRUE(newDb.tableExists("songs"));
    ASSERT_TRUE(newDb.tableExists("playlists"));

    SQLite::Statement versionQuery(newDb, "SELECT value FROM metadata WHERE key = 'db_version';");
    ASSERT_TRUE(versionQuery.executeStep());
    EXPECT_EQ(versionQuery.getColumn(0).getText(), std::string("3"));

    ASSERT_TRUE(hasColumn(newDb, "albums", "cover_path"));
    ASSERT_TRUE(hasColumn(newDb, "playlists", "cover_path"));
}