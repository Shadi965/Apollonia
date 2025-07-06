#include <gtest/gtest.h>
#include <SQLiteCpp/SQLiteCpp.h>

#include "db_manager.h"

#define DB_NAME ":memory:"

class DatabaseManagerTest : public ::testing::Test {
protected:
    std::string testDbPath = DB_NAME;
    
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
        ASSERT_TRUE(db.tableExists(table));

    SQLite::Statement query(db, "SELECT value FROM metadata WHERE key = 'db_version';");
    ASSERT_TRUE(query.executeStep());
    EXPECT_EQ(query.getColumn(0).getText(), std::string("5"));
}
