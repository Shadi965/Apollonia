#include <gtest/gtest.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <filesystem>

#include "db_manager.h"
#include "playlist_songs.h"

#define DB_NAME "test_playlist_song_dao.db"

class PlaylistSongDaoTest : public ::testing::Test {
protected:
    DatabaseManager dbManager;
    SQLite::Database& db;
    PlaylistSongDao pSongDao;

    PlaylistSongDaoTest() : dbManager(DB_NAME), db(dbManager.getDb()), pSongDao(db) {
        db.exec("INSERT OR IGNORE INTO playlists (name, cover_path) "
        "VALUES ('play1', NULL)");

        db.exec("INSERT OR IGNORE INTO playlists (name, cover_path) "
        "VALUES ('play2', '/path/cover.jpg')");

        db.exec("INSERT OR IGNORE INTO playlists (name, cover_path) "
            "VALUES ('play3', '/cov.img')");


        db.exec("INSERT OR IGNORE INTO songs (title, artist, composer, album_id, track, disc, date, copyright, genre) "
        "VALUES ('Test Song', 'Test Artist', 'Test Composer', NULL, 1, 1, '2025-04-13', 'Test Copyright', 'Test Genre')");

        db.exec("INSERT OR IGNORE INTO songs (title, artist, composer, album_id, track, disc, date, copyright, genre) "
        "VALUES ('Test Song1', 'Test Artist', 'Test Composer', NULL, 1, 1, '2025-04-13', 'Test Copyright', 'Test Genre')");

        db.exec("INSERT OR IGNORE INTO songs (title, artist, composer, album_id, track, disc, date, copyright, genre) "
        "VALUES ('Test Song2', 'Test Artist', 'Test Composer', NULL, 1, 1, '2025-04-13', 'Test Copyright', 'Test Genre')");
    }

    void TearDown() override {
        db.exec("DELETE FROM playlist_songs; VACUUM;");
        db.exec("UPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = 'playlist_songs';");
    }

    static void TearDownTestSuite() {
        if (std::filesystem::exists(DB_NAME))
            std::filesystem::remove(DB_NAME);
    }
};

TEST_F(PlaylistSongDaoTest, AddSongToPlaylist) {
    
    ASSERT_TRUE(pSongDao.addSongToPlaylist(1, 1));
    ASSERT_TRUE(pSongDao.addSongToPlaylist(1, 2));
    ASSERT_TRUE(pSongDao.addSongToPlaylist(2, 2));
    ASSERT_TRUE(pSongDao.addSongToPlaylist(2, 3));

    SQLite::Statement count(db, "SELECT COUNT(*) FROM playlist_songs;");
    count.executeStep();
    ASSERT_EQ(count.getColumn(0).getInt(), 4);

    SQLite::Statement query(db, "SELECT * FROM playlist_songs WHERE playlist_id = ? AND song_id = ?");
    query.bind(1, 1);
    query.bind(2, 1);
    ASSERT_TRUE(query.executeStep());
    query.reset();
    query.bind(1, 1);
    query.bind(2, 2);
    ASSERT_TRUE(query.executeStep());
    query.reset();
    query.bind(1, 2);
    query.bind(2, 2);
    ASSERT_TRUE(query.executeStep());
    query.reset();
    query.bind(1, 2);
    query.bind(2, 3);
    ASSERT_TRUE(query.executeStep());

    ASSERT_FALSE(pSongDao.addSongToPlaylist(1, 1));
    ASSERT_FALSE(pSongDao.addSongToPlaylist(2, 3));

    ASSERT_FALSE(pSongDao.addSongToPlaylist(5, 7));
    ASSERT_FALSE(pSongDao.addSongToPlaylist(6, 8));

    count.reset();
    count.executeStep();
    ASSERT_EQ(count.getColumn(0).getInt(), 4);
}
TEST_F(PlaylistSongDaoTest, RemoveSongFromPlaylist) {
    ASSERT_FALSE(pSongDao.removeSongFromPlaylist(1, 1));
    ASSERT_FALSE(pSongDao.removeSongFromPlaylist(6, 9));
    
    pSongDao.addSongToPlaylist(1, 1);
    pSongDao.addSongToPlaylist(1, 2);
    pSongDao.addSongToPlaylist(2, 2);
    pSongDao.addSongToPlaylist(2, 3);

    SQLite::Statement count(db, "SELECT COUNT(*) FROM playlist_songs;");
    count.executeStep();
    ASSERT_EQ(count.getColumn(0).getInt(), 4);

    ASSERT_TRUE(pSongDao.removeSongFromPlaylist(1, 1));
    count.reset();
    count.executeStep();
    ASSERT_EQ(count.getColumn(0).getInt(), 3);

    ASSERT_TRUE(pSongDao.removeSongFromPlaylist(2, 3));
    count.reset();
    count.executeStep();
    ASSERT_EQ(count.getColumn(0).getInt(), 2);

    ASSERT_FALSE(pSongDao.removeSongFromPlaylist(2, 3));
    count.reset();
    count.executeStep();
    ASSERT_EQ(count.getColumn(0).getInt(), 2);
}

TEST_F(PlaylistSongDaoTest, GetSongsInPlaylist) {
    std::vector<int> pSongs = pSongDao.getSongsInPlaylist(1);
    ASSERT_EQ(pSongs.size(), 0);
    pSongs = pSongDao.getSongsInPlaylist(5);
    ASSERT_EQ(pSongs.size(), 0);

    pSongDao.addSongToPlaylist(1, 1);
    pSongDao.addSongToPlaylist(1, 2);
    pSongDao.addSongToPlaylist(1, 3);
    pSongDao.addSongToPlaylist(2, 2);
    pSongDao.addSongToPlaylist(2, 3);

    pSongs = pSongDao.getSongsInPlaylist(1);
    ASSERT_EQ(pSongs.size(), 3);
    EXPECT_EQ(pSongs[0], 1);
    EXPECT_EQ(pSongs[1], 2);
    EXPECT_EQ(pSongs[2], 3);

    pSongs = pSongDao.getSongsInPlaylist(2);
    ASSERT_EQ(pSongs.size(), 2);
    EXPECT_EQ(pSongs[0], 2);
    EXPECT_EQ(pSongs[1], 3);
}

TEST_F(PlaylistSongDaoTest, OnDeleteCascade) {    
    pSongDao.addSongToPlaylist(1, 1);
    pSongDao.addSongToPlaylist(1, 2);
    pSongDao.addSongToPlaylist(1, 3);
    pSongDao.addSongToPlaylist(2, 2);
    pSongDao.addSongToPlaylist(2, 3);

    ASSERT_EQ(pSongDao.getSongsInPlaylist(1).size(), 3);
    ASSERT_EQ(pSongDao.getSongsInPlaylist(2).size(), 2);

    db.exec("DELETE FROM songs WHERE id = 2");
    ASSERT_EQ(pSongDao.getSongsInPlaylist(1).size(), 2);
    ASSERT_EQ(pSongDao.getSongsInPlaylist(2).size(), 1);

    db.exec("DELETE FROM playlists WHERE id = 1");
    ASSERT_EQ(pSongDao.getSongsInPlaylist(1).size(), 0);
    ASSERT_EQ(pSongDao.getSongsInPlaylist(2).size(), 1);
}