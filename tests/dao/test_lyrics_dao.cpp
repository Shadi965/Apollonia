#include <gtest/gtest.h>
#include <SQLiteCpp/SQLiteCpp.h>

#include "db_manager.h"
#include "lyrics.h"

#define DB_NAME ":memory:"

class LyricsDaoTest : public ::testing::Test {
protected:
    DatabaseManager dbManager;
    SQLite::Database& db;
    LyricsDao lyricsDao;

    LyricsDaoTest() : dbManager(DB_NAME), db(dbManager.getDb()), lyricsDao(db) {
        db.exec("INSERT OR IGNORE INTO songs (title, artist, composer, album_id, track, disc, date, copyright, genre) "
        "VALUES ('Test Song', 'Test Artist', 'Test Composer', NULL, 1, 1, '2025-04-13', 'Test Copyright', 'Test Genre')");

        db.exec("INSERT OR IGNORE INTO songs (title, artist, composer, album_id, track, disc, date, copyright, genre) "
        "VALUES ('Test Song1', 'Test Artist', 'Test Composer', NULL, 1, 1, '2025-04-13', 'Test Copyright', 'Test Genre')");

        db.exec("INSERT OR IGNORE INTO songs (title, artist, composer, album_id, track, disc, date, copyright, genre) "
        "VALUES ('Test Song2', 'Test Artist', 'Test Composer', NULL, 1, 1, '2025-04-13', 'Test Copyright', 'Test Genre')");
    }

    void TearDown() override {
        db.exec("DELETE FROM lyrics; VACUUM;");
        db.exec("UPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = 'lyrics';");
    }
};

TEST_F(LyricsDaoTest, AddLyricLine) {
    
    ASSERT_TRUE(lyricsDao.addLyricLine(1, 0, "line1"));

    SQLite::Statement query(db, "SELECT * FROM lyrics WHERE song_id = ?");
    query.bind(1, 1);
    query.executeStep();
    EXPECT_EQ(query.getColumn(0).getInt(), 1);
    EXPECT_EQ(query.getColumn(1).getInt(), 0);
    EXPECT_EQ(query.getColumn(2).getString(), "line1");

    ASSERT_FALSE(lyricsDao.addLyricLine(1, 0, "line2"));
    ASSERT_TRUE(lyricsDao.addLyricLine(1, 12, "line1"));
    ASSERT_TRUE(lyricsDao.addLyricLine(2, 0, "line1"));
    ASSERT_FALSE(lyricsDao.addLyricLine(16, 0, "line1"));
}

TEST_F(LyricsDaoTest, GetLyricsForSong) {
    std::vector<LyricLineEntity> lyrics = lyricsDao.getLyricsForSong(1);
    EXPECT_EQ(lyrics.size(), 0);

    lyricsDao.addLyricLine(1, 0, "line1");
    lyricsDao.addLyricLine(1, 13, "line2");
    lyricsDao.addLyricLine(1, 17, "line3");
    lyricsDao.addLyricLine(1, 22, "line4");

    lyricsDao.addLyricLine(2, 12, "line1");
    lyricsDao.addLyricLine(2, 18, "line1");
    lyricsDao.addLyricLine(2, 30, "line4");

    
    lyrics = lyricsDao.getLyricsForSong(1);
    ASSERT_EQ(lyrics.size(), 4);

    EXPECT_EQ(lyrics[0].song_id, 1);
    EXPECT_EQ(lyrics[0].time_ms, 0);
    EXPECT_EQ(lyrics[0].line, "line1");

    EXPECT_EQ(lyrics[3].song_id, 1);
    EXPECT_EQ(lyrics[3].time_ms, 22);
    EXPECT_EQ(lyrics[3].line, "line4");

    lyrics = lyricsDao.getLyricsForSong(2);
    ASSERT_EQ(lyrics.size(), 3);

    EXPECT_EQ(lyrics[0].song_id, 2);
    EXPECT_EQ(lyrics[0].time_ms, 12);
    EXPECT_EQ(lyrics[0].line, "line1");

    EXPECT_EQ(lyrics[2].song_id, 2);
    EXPECT_EQ(lyrics[2].time_ms, 30);
    EXPECT_EQ(lyrics[2].line, "line4");
}

TEST_F(LyricsDaoTest, OnDeleteCascade) {    
    lyricsDao.addLyricLine(1, 0, "line1");
    lyricsDao.addLyricLine(1, 13, "line2");
    lyricsDao.addLyricLine(1, 17, "line3");
    lyricsDao.addLyricLine(1, 22, "line4");

    lyricsDao.addLyricLine(2, 12, "line1");
    lyricsDao.addLyricLine(2, 18, "line1");
    lyricsDao.addLyricLine(2, 30, "line4");

    ASSERT_EQ(lyricsDao.getLyricsForSong(1).size(), 4);
    ASSERT_EQ(lyricsDao.getLyricsForSong(2).size(), 3);

    db.exec("DELETE FROM songs WHERE id = 1");
    ASSERT_EQ(lyricsDao.getLyricsForSong(1).size(), 0);
    ASSERT_EQ(lyricsDao.getLyricsForSong(2).size(), 3);

    db.exec("DELETE FROM songs WHERE id = 2");
    ASSERT_EQ(lyricsDao.getLyricsForSong(2).size(), 0);
}