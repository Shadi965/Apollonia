#include <gtest/gtest.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <filesystem>

#include "db_manager.h"
#include "songs_meta.h"

#define DB_NAME "test_song_meta_dao.db"

class SongMetaDaoTest : public ::testing::Test {
protected:
    DatabaseManager dbManager;
    SQLite::Database& db;
    SongMetaDao songMetaDao;

    SongMetaDaoTest() : dbManager(DB_NAME), db(dbManager.getDb()), songMetaDao(db) {
        db.exec("INSERT OR IGNORE INTO songs (title, artist, composer, album_id, track, disc, date, copyright, genre) "
        "VALUES ('Test Song', 'Test Artist', 'Test Composer', NULL, 1, 1, '2025-04-13', 'Test Copyright', 'Test Genre')");

        db.exec("INSERT OR IGNORE INTO songs (title, artist, composer, album_id, track, disc, date, copyright, genre) "
        "VALUES ('Test Song1', 'Test Artist', 'Test Composer', NULL, 1, 1, '2025-04-13', 'Test Copyright', 'Test Genre')");

        db.exec("INSERT OR IGNORE INTO songs (title, artist, composer, album_id, track, disc, date, copyright, genre) "
        "VALUES ('Test Song2', 'Test Artist', 'Test Composer', NULL, 1, 1, '2025-04-13', 'Test Copyright', 'Test Genre')");
    }

    void TearDown() override {
        db.exec("DELETE FROM songs_meta; VACUUM;");
        db.exec("UPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = 'songs_meta';");
    }

    static void TearDownTestSuite() {
        if (std::filesystem::exists(DB_NAME))
            std::filesystem::remove(DB_NAME);
    }

    static bool compareSongMetas(const SongMetaEntity& first, const SongMetaEntity& second) {
        return std::tie(
            first.song_id,
            first.duration,
            first.bitrate,
            first.channels,
            first.sample_rate,
            first.last_modified,
            first.file
        ) == std::tie(
            second.song_id,
            second.duration,
            second.bitrate,
            second.channels,
            second.sample_rate,
            second.last_modified,
            second.file
        );
    }
};

TEST_F(SongMetaDaoTest, InsertSongData) {
    int songId = 1;
    SongMetaEntity songMeta = {songId, 15, 16, 1, 4, 1, "/file"};
    
    ASSERT_TRUE(songMetaDao.insertSongData(songMeta));

    SQLite::Statement query(db, "SELECT * FROM songs_meta WHERE song_id = ?");
    query.bind(1, songId);
    query.executeStep();
    EXPECT_EQ(query.getColumn(0).getInt(), songMeta.song_id);
    EXPECT_EQ(query.getColumn(1).getInt(), songMeta.duration);
    EXPECT_EQ(query.getColumn(2).getInt(), songMeta.bitrate);
    EXPECT_EQ(query.getColumn(3).getInt(), songMeta.channels);
    EXPECT_EQ(query.getColumn(4).getInt(), songMeta.sample_rate);
    EXPECT_EQ(query.getColumn(5).getInt(), songMeta.last_modified);
    EXPECT_EQ(query.getColumn(6).getString(), songMeta.file);

    songMeta.file = "/file2";
    ASSERT_FALSE(songMetaDao.insertSongData(songMeta));

    songMeta.song_id = 2;
    songMeta.file = "/file";
    ASSERT_FALSE(songMetaDao.insertSongData(songMeta));

    songMeta.song_id = 2;
    songMeta.file = "/file2";
    ASSERT_TRUE(songMetaDao.insertSongData(songMeta));

    songMeta.song_id = 15;
    songMeta.file = "/file2";
    ASSERT_FALSE(songMetaDao.insertSongData(songMeta));

    songMeta.song_id = 15;
    songMeta.file = "/file15";
    ASSERT_FALSE(songMetaDao.insertSongData(songMeta));
}

TEST_F(SongMetaDaoTest, GetSongDataById) {
    int songId = 1;
    SongMetaEntity songMeta = {songId, 15, 16, 1, 4, 1, "/file"};
    songMetaDao.insertSongData(songMeta);

    SongMetaEntity retrievedSongData = songMetaDao.getSongDataById(songId);
    EXPECT_TRUE(compareSongMetas(retrievedSongData, songMeta));

    ASSERT_THROW(songMetaDao.getSongDataById(0), SongNotFoundException);
    ASSERT_THROW(songMetaDao.getSongDataById(2), SongNotFoundException);
    ASSERT_THROW(songMetaDao.getSongDataById(5), SongNotFoundException);
}
TEST_F(SongMetaDaoTest, GetAllSongsData) {
    std::vector<SongMetaEntity> songs = songMetaDao.getAllSongsData();
    EXPECT_EQ(songs.size(), 0);

    SongMetaEntity songMeta1 = {1, 15, 16, 1, 4, 1, "/file"};
    SongMetaEntity songMeta2 = {2, 15, 16, 1, 4, 1, "/file1"};
    SongMetaEntity songMeta3 = {3, 15, 16, 1, 4, 1, "/file2"};



    songMetaDao.insertSongData(songMeta1);
    songMetaDao.insertSongData(songMeta2);
    songMetaDao.insertSongData(songMeta3);
    
    songs = songMetaDao.getAllSongsData();
    
    ASSERT_EQ(songs.size(), 3);
    EXPECT_TRUE(compareSongMetas(songs[0], songMeta1));
    EXPECT_TRUE(compareSongMetas(songs[1], songMeta2));
    EXPECT_TRUE(compareSongMetas(songs[2], songMeta3));
}
TEST_F(SongMetaDaoTest, GetSongDuration) {
    int songId = 1;
    SongMetaEntity songMeta = {songId, 15, 16, 1, 4, 1, "/file"};
    songMetaDao.insertSongData(songMeta);

    int duration = songMetaDao.getSongDuration(songId);
    EXPECT_EQ(duration, songMeta.duration);

    ASSERT_THROW(songMetaDao.getSongDuration(0), SongNotFoundException);
    ASSERT_THROW(songMetaDao.getSongDuration(2), SongNotFoundException);
    ASSERT_THROW(songMetaDao.getSongDuration(5), SongNotFoundException);
}
TEST_F(SongMetaDaoTest, GetFilePathBySongId) {
    int songId = 1;
    SongMetaEntity songMeta = {songId, 15, 16, 1, 4, 1, "/file"};
    songMetaDao.insertSongData(songMeta);

    std::string path = songMetaDao.getFilePathBySongId(songId);
    EXPECT_EQ(path, songMeta.file);

    ASSERT_THROW(songMetaDao.getFilePathBySongId(0), SongNotFoundException);
    ASSERT_THROW(songMetaDao.getFilePathBySongId(2), SongNotFoundException);
    ASSERT_THROW(songMetaDao.getFilePathBySongId(5), SongNotFoundException);
}

TEST_F(SongMetaDaoTest, OnDeleteCascade) {    
    songMetaDao.insertSongData({1, 15, 16, 1, 4, 1, "/file"});
    songMetaDao.insertSongData({2, 15, 16, 1, 4, 1, "/file2"});

    ASSERT_EQ(songMetaDao.getAllSongsData().size(), 2);

    db.exec("DELETE FROM songs WHERE id = 1");

    ASSERT_EQ(songMetaDao.getAllSongsData().size(), 1);
}