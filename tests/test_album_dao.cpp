#include <gtest/gtest.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <filesystem>

#include "db_manager.h"
#include "albums.h"

class AlbumDaoTest : public ::testing::Test {
protected:
    DatabaseManager dbManager;
    SQLite::Database& db;
    AlbumDao albumDao;

    AlbumDaoTest() : dbManager("test_music.db"), db(dbManager.getDb()), albumDao(db) {}

    void TearDown() override {
        db.exec("DELETE FROM albums; VACUUM;");
        db.exec("UPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = 'albums';");
    }

    static void TearDownTestSuite() {
        if (std::filesystem::exists("test_music.db"))
            std::filesystem::remove("test_music.db");
    }

    static bool compareAlbums(const AlbumEntity& first, const AlbumEntity& second) {
        return std::tie(
            first.id,
            first.title,
            first.artist,
            first.track_count,
            first.disc_count,
            first.compilation,
            first.date,
            first.copyright,
            first.genre,
            first.cover_path
        ) == std::tie(
            second.id,
            second.title,
            second.artist,
            second.track_count,
            second.disc_count,
            second.compilation,
            second.date,
            second.copyright,
            second.genre,
            second.cover_path
        );
    }
};

TEST_F(AlbumDaoTest, InsertAlbum) {
    AlbumEntity album = {0, "Test Album", "Test Artist", 10, 1, false, "2025-04-13", "Test Copyright", "Test Genre", "/home/test.jpg"};
    
    int albumId = albumDao.insertAlbum(album);
    EXPECT_EQ(albumId, 1);

    SQLite::Statement query(db, "SELECT * FROM albums WHERE id = ?");
    query.bind(1, albumId);
    query.executeStep();
    EXPECT_EQ(query.getColumn(0).getInt(), 1);
    EXPECT_EQ(query.getColumn(1).getString(), album.title);
    EXPECT_EQ(query.getColumn(2).getString(), album.artist);
    EXPECT_EQ(query.getColumn(3).getInt(), album.track_count);
    EXPECT_EQ(query.getColumn(4).getInt(), album.disc_count);
    EXPECT_EQ(query.getColumn(5).getInt(), album.compilation);
    EXPECT_EQ(query.getColumn(6).getString(), album.date);
    EXPECT_EQ(query.getColumn(7).getString(), album.copyright);
    EXPECT_EQ(query.getColumn(8).getString(), album.genre);
    EXPECT_EQ(query.getColumn(9).getString(), album.cover_path);

    albumId = albumDao.insertAlbum(album);
    ASSERT_EQ(albumId, 0);

    albumId = albumDao.insertAlbum({0, "Test Album 2", "Test Artist 2", 10, 1, false, "2025-04-13", "Test Copyright", "Test Genre", ""});
    ASSERT_EQ(albumId, 2);
}

TEST_F(AlbumDaoTest, CheckAlbumExists) {
    int id = albumDao.checkAlbumExists("Test Artist", "Test Album 1");
    EXPECT_EQ(id, 0);

    AlbumEntity album1{1, "Test Album 1", "Test Artist", 10, 1, false, "2025-04-13", "Test Copyright", "Test Genre", ""};
    AlbumEntity album2{2, "Test Album 2", "Testisto Artisto", 2, 1, true, "2025-12-13", "Test Copyright", "Test Genre", ""};
    AlbumEntity album3{3, "Test Album 4", "Testo Artist", 12, 3, false, "2035-04-13", "Testo Copyrighto", "Test Genro", ""};

    albumDao.insertAlbum(album1);
    albumDao.insertAlbum(album2);
    albumDao.insertAlbum(album3);

    id = albumDao.checkAlbumExists("Not exist", "Not exist");
    EXPECT_EQ(id, 0);
    
    id = albumDao.checkAlbumExists(album1.artist, album1.title);
    EXPECT_EQ(id, album1.id);
    
    id = albumDao.checkAlbumExists(album2.artist, album2.title);
    EXPECT_EQ(id, album2.id);

    id = albumDao.checkAlbumExists(album3.artist, album3.title);
    EXPECT_EQ(id, album3.id);

    id = albumDao.checkAlbumExists("Exist(not)", "Exist(not)");
    EXPECT_EQ(id, 0);
}


TEST_F(AlbumDaoTest, GetAlbumById) {
    AlbumEntity album = {0, "Test Album", "Test Artist", 10, 1, false, "2025-04-13", "Test Copyright", "Test Genre", ""};
    int albumId = albumDao.insertAlbum(album);
    album.id = albumId;

    AlbumEntity retrievedAlbum = albumDao.getAlbumById(albumId);
    EXPECT_TRUE(compareAlbums(retrievedAlbum, album));

    ASSERT_THROW(albumDao.getAlbumById(0), AlbumNotFoundException);
    ASSERT_THROW(albumDao.getAlbumById(2), AlbumNotFoundException);
}
TEST_F(AlbumDaoTest, GetAllAlbums) {
    std::vector<AlbumEntity> albums = albumDao.getAllAlbums();
    EXPECT_EQ(albums.size(), 0);

    AlbumEntity album1{1, "Test Album 1", "Test Artist", 10, 1, false, "2025-04-13", "Test Copyright", "Test Genre", ""};
    AlbumEntity album2{2, "Test Album 2", "Testisto Artisto", 2, 1, true, "2025-12-13", "Test Copyright", "Test Genre", ""};
    AlbumEntity album3{3, "Test Album 4", "Testo Artist", 12, 3, false, "2035-04-13", "Testo Copyrighto", "Test Genro", ""};

    albumDao.insertAlbum(album1);
    albumDao.insertAlbum(album2);
    albumDao.insertAlbum(album3);
    
    albums = albumDao.getAllAlbums();
    
    ASSERT_EQ(albums.size(), 3);
    EXPECT_TRUE(compareAlbums(albums[0], album1));
    EXPECT_TRUE(compareAlbums(albums[1], album2));
    EXPECT_TRUE(compareAlbums(albums[2], album3));
}
TEST_F(AlbumDaoTest, GetAllAlbumsIds) {
    std::vector<int> ids = albumDao.getAllAlbumsIds();
    EXPECT_EQ(ids.size(), 0);

    AlbumEntity album1{1, "Test Album 1", "Test Artist", 10, 1, false, "2025-04-13", "Test Copyright", "Test Genre", ""};
    AlbumEntity album2{2, "Test Album 2", "Testisto Artisto", 2, 1, true, "2025-12-13", "Test Copyright", "Test Genre", ""};
    AlbumEntity album3{3, "Test Album 4", "Testo Artist", 12, 3, false, "2035-04-13", "Testo Copyrighto", "Test Genro", ""};

    albumDao.insertAlbum(album1);
    albumDao.insertAlbum(album2);
    albumDao.insertAlbum(album3);
    
    ids = albumDao.getAllAlbumsIds();
    
    ASSERT_EQ(ids.size(), 3);
    EXPECT_EQ(ids[0], album1.id);
    EXPECT_EQ(ids[1], album2.id);
    EXPECT_EQ(ids[2], album3.id);
}

TEST_F(AlbumDaoTest, GetAlbumTitleById) {
    AlbumEntity album = {0, "Test Album", "Test Artist", 10, 1, false, "2025-04-13", "Test Copyright", "Test Genre", ""};
    int albumId = albumDao.insertAlbum(album);

    std::string title = albumDao.getAlbumTitleById(albumId);
    EXPECT_EQ(title, album.title);

    ASSERT_THROW(albumDao.getAlbumTitleById(0), AlbumNotFoundException);
    ASSERT_THROW(albumDao.getAlbumTitleById(2), AlbumNotFoundException);
}
TEST_F(AlbumDaoTest, GetAlbumCoverPathById) {
    AlbumEntity album = {0, "Test Album", "Test Artist", 10, 1, false, "2025-04-13", "Test Copyright", "Test Genre", "/home/test"};
    int albumId = albumDao.insertAlbum(album);

    std::string path = albumDao.getAlbumCoverPathById(albumId);
    EXPECT_EQ(path, album.cover_path);

    db.exec("UPDATE albums SET cover_path = NULL WHERE id = 1");

    path = albumDao.getAlbumCoverPathById(albumId);
    EXPECT_EQ(path, "");

    ASSERT_THROW(albumDao.getAlbumCoverPathById(0), AlbumNotFoundException);
    ASSERT_THROW(albumDao.getAlbumCoverPathById(2), AlbumNotFoundException);
}


TEST_F(AlbumDaoTest, UpdateAlbumDateAndGenre) {
    AlbumEntity album = {0, "Test Album", "Test Artist", 10, 1, false, "2025-04-13", "Test Copyright", "Test Genre", ""};
    int albumId = albumDao.insertAlbum(album);

    std::string newDate = "2025-04-14";
    std::string newGenre = "New Genre";
    bool updated = albumDao.updateAlbumDateAndGenre(albumId, newDate, newGenre);
    ASSERT_TRUE(updated);

    AlbumEntity updatedAlbum = albumDao.getAlbumById(albumId);
    EXPECT_EQ(updatedAlbum.date, newDate);
    EXPECT_EQ(updatedAlbum.genre, newGenre);
}
TEST_F(AlbumDaoTest, UpdateAlbumCoverPath) {
    AlbumEntity album = {0, "Test Album", "Test Artist", 10, 1, false, "2025-04-13", "Test Copyright", "Test Genre", ""};
    int albumId = albumDao.insertAlbum(album);

    std::string newCoverPath = "/home/user/pics/cover.jpg";
    bool updated = albumDao.updateAlbumCoverPath(albumId, newCoverPath);
    ASSERT_TRUE(updated);

    AlbumEntity updatedAlbum = albumDao.getAlbumById(albumId);
    EXPECT_EQ(updatedAlbum.cover_path, newCoverPath);
}
