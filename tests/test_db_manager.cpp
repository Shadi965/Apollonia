#include <gtest/gtest.h>
#include "db_manager.h"
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>

class DatabaseManagerTest : public ::testing::Test {
protected:
    std::string testDbPath = "test_music.db";

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
};

TEST_F(DatabaseManagerTest, DBIsCreatedAndInitialized) {
    DatabaseManager dbManager(testDbPath);
    SQLite::Database& db = dbManager.getDb();

    std::vector<std::string> tables = {
        "songs", "songs_meta", "albums", "playlists",
        "lyrics", "playlist_songs", "metadata"
    };

    for (const auto& table : tables)
        ASSERT_TRUE(db.tableExists(table)) << "Table '" << table << "' does not exist!";

    SQLite::Statement query(db, "SELECT value FROM metadata WHERE key = 'db_version';");
    ASSERT_TRUE(query.executeStep());
    EXPECT_EQ(query.getColumn(0).getText(), std::string("2"));
}

TEST_F(DatabaseManagerTest, FromV1ToV2Upgrade) {
    SQLite::Database db(testDbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    db.exec("CREATE TABLE albums (id INTEGER PRIMARY KEY, title TEXT,"
                                 "artist TEXT, track_count INTEGER, "
                                 "disc_count INTEGER, compilation INTEGER, "
                                 "date TEXT, copyright TEXT, genre TEXT);");
    db.exec("CREATE TABLE playlists (id INTEGER PRIMARY KEY, name TEXT);");
    db.exec("CREATE TABLE metadata (key TEXT PRIMARY KEY, value TEXT);");
    db.exec("INSERT INTO metadata (key, value) VALUES ('db_version', '1');");

    DatabaseManager dbManager(testDbPath);
    SQLite::Database& newDb = dbManager.getDb();

    ASSERT_TRUE(newDb.tableExists("albums"));
    ASSERT_TRUE(newDb.tableExists("playlists"));

    SQLite::Statement versionQuery(newDb, "SELECT value FROM metadata WHERE key = 'db_version';");
    ASSERT_TRUE(versionQuery.executeStep());
    EXPECT_EQ(versionQuery.getColumn(0).getText(), std::string("2"));

    ASSERT_TRUE(hasColumn(newDb, "albums", "cover_path")) 
        << "'albums' table missing 'cover_path' column after upgrade.";
    ASSERT_TRUE(hasColumn(newDb, "playlists", "cover_path")) 
        << "'playlists' table missing 'cover_path' column after upgrade.";
}
