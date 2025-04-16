#include <gtest/gtest.h>
#include <SQLiteCpp/SQLiteCpp.h>

#include "db_manager.h"
#include "songs.h"

#define DB_NAME ":memory:"

class SongDaoTest : public ::testing::Test {
protected:
    DatabaseManager dbManager;
    SQLite::Database& db;
    SongDao songDao;

    SongDaoTest() : dbManager(DB_NAME), db(dbManager.getDb()), songDao(db) {
        db.exec("INSERT OR IGNORE INTO albums (title, artist, track_count, disc_count, compilation, date, copyright, genre, cover_path) "
            "VALUES ('Test Title', 'Test Artist', 3, 1, 1, '2025-04-13', 'Test Copyright', 'Test Genre', '/path/')");
    }

    void TearDown() override {
        db.exec("DELETE FROM songs; VACUUM;");
        db.exec("UPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = 'songs';");
    }

    static bool compareSongs(const SongEntity& first, const SongEntity& second) {
        return std::tie(
            first.id,
            first.title,
            first.artist,
            first.composer,
            first.album_id,
            first.track,
            first.disc,
            first.date,
            first.copyright,
            first.genre
        ) == std::tie(
            second.id,
            second.title,
            second.artist,
            second.composer,
            second.album_id,
            second.track,
            second.disc,
            second.date,
            second.copyright,
            second.genre
        );
    }
};

TEST_F(SongDaoTest, InsertSong) {
    SongEntity song = {0, "Test Song", "Test Artist", "Test Composer", 0, 1, 1, "2025-04-13", "Test Copyright", "Test Genre"};
    
    int songId = songDao.insertSong(song);
    EXPECT_EQ(songId, 1);

    SQLite::Statement query(db, "SELECT * FROM songs WHERE id = ?");
    query.bind(1, songId);
    query.executeStep();
    EXPECT_EQ(query.getColumn(0).getInt(), 1);
    EXPECT_EQ(query.getColumn(1).getString(), song.title);
    EXPECT_EQ(query.getColumn(2).getString(), song.artist);
    EXPECT_EQ(query.getColumn(3).getString(), song.composer);
    EXPECT_EQ(query.getColumn(4).getInt(),    song.album_id);
    EXPECT_EQ(query.getColumn(5).getInt(),    song.track);
    EXPECT_EQ(query.getColumn(6).getInt(),    song.disc);
    EXPECT_EQ(query.getColumn(7).getString(), song.date);
    EXPECT_EQ(query.getColumn(8).getString(), song.copyright);
    EXPECT_EQ(query.getColumn(9).getString(), song.genre);

    song.album_id = 14;
    ASSERT_THROW(songDao.insertSong(song), AlbumNotFoundException);

    song.album_id = 1;

    songId = songDao.insertSong(song);
    SQLite::Statement q2(db, "SELECT * FROM songs WHERE id = ?");
    q2.bind(1, songId);
    q2.executeStep();
    EXPECT_EQ(q2.getColumn(4).getInt(), song.album_id);

    ASSERT_THROW(songDao.insertSong({0, "", "", "", 1, 1, 1, "", "", ""}), SongAlreadyExists);
    ASSERT_NO_THROW(songDao.insertSong({0, "", "", "", 1, 2, 1, "", "", ""}));
    ASSERT_NO_THROW(songDao.insertSong({0, "", "", "", 1, 1, 2, "", "", ""}));
}
TEST_F(SongDaoTest, DeleteSong) {
    EXPECT_FALSE(songDao.deleteSongById(1));

    SongEntity song = {0, "Test Song", "Test Artist", "Test Composer", 0, 1, 1, "2025-04-13", "Test Copyright", "Test Genre"};
    
    int songId = songDao.insertSong(song);
    EXPECT_TRUE(songDao.deleteSongById(songId));
}

TEST_F(SongDaoTest, GetSongById) {
    SongEntity song = {0, "Test Song", "Test Artist", "Test Composer", 0, 1, 1, "2025-04-13", "Test Copyright", "Test Genre"};
    int songId = songDao.insertSong(song);
    song.id = songId;

    SongEntity retrievedSong = songDao.getSongById(songId);
    EXPECT_TRUE(compareSongs(retrievedSong, song));

    ASSERT_THROW(songDao.getSongById(0), SongNotFoundException);
    ASSERT_THROW(songDao.getSongById(2), SongNotFoundException);
}
TEST_F(SongDaoTest, GetAllSongs) {
    std::vector<SongEntity> songs = songDao.getAllSongs();
    EXPECT_EQ(songs.size(), 0);

    SongEntity song1 = {0, "Test Song1", "Test Artist", "Test Composer", 0, 1, 1, "2025-04-13", "Test Copyright", "Test Genre"};
    SongEntity song2 = {0, "Test Song2", "Test Artist2", "Test Composer", 1, 1, 1, "2025-04-13", "Test Copyright", "Test Genre"};
    SongEntity song3 = {0, "Test Song3", "Test Artist2", "Test Composer", 1, 2, 1, "2025-04-13", "Test Copyright", "Test Genre"};


    song1.id = songDao.insertSong(song1);
    song2.id = songDao.insertSong(song2);
    song3.id = songDao.insertSong(song3);
    
    songs = songDao.getAllSongs();
    
    ASSERT_EQ(songs.size(), 3);
    EXPECT_TRUE(compareSongs(songs[0], song1));
    EXPECT_TRUE(compareSongs(songs[1], song2));
    EXPECT_TRUE(compareSongs(songs[2], song3));
}

TEST_F(SongDaoTest, GetNewestSongDateInAlbum) {
    int albumId = 1;
    SongEntity song1 = {0, "Test Song", "Test Artist", "Test Composer", albumId, 1, 1, "2025-04-13", "Test Copyright", "Test Genre"};
    SongEntity song2 = {0, "Test Song2", "Test Artist", "Test Composer", albumId, 2, 1, "2024-04-13", "Test Copyright", "Test Genre"};
    SongEntity song3 = {0, "Test Song3", "Test Artist", "Test Composer", albumId, 3, 1, "2025-04-17", "Test Copyright", "Test Genre"};
    SongEntity song4 = {0, "Test Song4", "Test Artist", "Test Composer", 0, 3, 1, "2025-04-17", "Test Copyright", "Test Genre"};
    songDao.insertSong(song1);
    songDao.insertSong(song2);
    songDao.insertSong(song3);
    songDao.insertSong(song4);

    EXPECT_EQ(songDao.getNewestSongDateInAlbum(1), song3.date);

    ASSERT_EQ(songDao.getNewestSongDateInAlbum(15), "");
}
TEST_F(SongDaoTest, GetMostFrequentGenreInAlbum) {
    int albumId = 1;
    SongEntity song1 = {0, "Test Song", "Test Artist", "Test Composer", albumId, 1, 1, "2025-04-13", "Test Copyright", "Test Genre1"};
    SongEntity song2 = {0, "Test Song2", "Test Artist", "Test Composer", albumId, 2, 1, "2024-04-13", "Test Copyright", "Test Genre1"};
    SongEntity song3 = {0, "Test Song3", "Test Artist", "Test Composer", albumId, 3, 1, "2025-04-17", "Test Copyright", "Test Genre2"};
    SongEntity song4 = {0, "Test Song4", "Test Artist", "Test Composer", 0, 3, 1, "2025-04-17", "Test Copyright", "Test Genre"};
    songDao.insertSong(song1);
    songDao.insertSong(song2);
    songDao.insertSong(song3);
    songDao.insertSong(song4);

    EXPECT_EQ(songDao.getMostFrequentGenreInAlbum(1), song1.genre);

    ASSERT_EQ(songDao.getNewestSongDateInAlbum(15), "");
}
TEST_F(SongDaoTest, GetTitleArtistAlbumTuples) {
    std::vector<SongEntity> songs = songDao.getTitleArtistAlbumTuples();
    EXPECT_EQ(songs.size(), 0);

    SongEntity song1 = {0, "Test Song", "Test Artist", "Test Composer", 1, 1, 1, "2025-04-13", "Test Copyright", "Test Genre1"};
    SongEntity song2 = {0, "Test Song2", "Test Artist", "Test Composer", 1, 2, 1, "2024-04-13", "Test Copyright", "Test Genre1"};
    SongEntity song3 = {0, "Test Song3", "Test Artist", "Test Composer", 1, 3, 1, "2025-04-17", "Test Copyright", "Test Genre2"};
    SongEntity song4 = {0, "Test Song4", "Test Artist2", "Test Composer", 0, 1, 1, "2025-04-17", "Test Copyright2", "Test Genre1"};
    SongEntity song5 = {0, "Test Song5", "Test Artist3", "Test Composer", 0, 1, 1, "2025-04-17", "Test Copyright3", "Test Genre3"};
    song1.id = songDao.insertSong(song1);
    song2.id = songDao.insertSong(song2);
    song3.id = songDao.insertSong(song3);
    song4.id = songDao.insertSong(song4);
    song5.id = songDao.insertSong(song5);

    songs = songDao.getTitleArtistAlbumTuples();

    ASSERT_EQ(songs.size(), 5);
    
    std::vector<SongEntity> s;
    s.reserve(5);
    s.push_back(song1);
    s.push_back(song2);
    s.push_back(song3);
    s.push_back(song4);
    s.push_back(song5);
    for (int i = 0; i < songs.size(); i++) {
        EXPECT_EQ(songs[i].id, s[i].id);
        EXPECT_EQ(songs[i].title, s[i].title);
        EXPECT_EQ(songs[i].artist, s[i].artist);
        EXPECT_EQ(songs[i].album_id, s[i].album_id);
    }   
}
TEST_F(SongDaoTest, GetSongsFromAlbum) {
    std::vector<int> songs = songDao.getSongsFromAlbum(1);
    EXPECT_EQ(songs.size(), 0);

    SongEntity song1 = {0, "Test Song1", "Test Artist", "Test Composer", 0, 1, 1, "2025-04-13", "Test Copyright", "Test Genre"};
    SongEntity song2 = {0, "Test Song2", "Test Artist2", "Test Composer", 1, 1, 1, "2025-04-13", "Test Copyright", "Test Genre"};
    SongEntity song3 = {0, "Test Song3", "Test Artist2", "Test Composer", 1, 2, 1, "2025-04-13", "Test Copyright", "Test Genre"};


    song1.id = songDao.insertSong(song1);
    song2.id = songDao.insertSong(song2);
    song3.id = songDao.insertSong(song3);
    
    songs = songDao.getSongsFromAlbum(1);
    
    ASSERT_EQ(songs.size(), 2);
    EXPECT_EQ(songs[0], 2);
    EXPECT_EQ(songs[1], 3);
}
