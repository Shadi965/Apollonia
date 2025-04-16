#include <gtest/gtest.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <filesystem>

#include "db_manager.h"
#include "playlists.h"

#define DB_NAME "test_playlist_dao.db"

class PlaylistDaoTest : public ::testing::Test {
protected:
    DatabaseManager dbManager;
    SQLite::Database& db;
    PlaylistDao playlistDao;

    PlaylistDaoTest() : dbManager(DB_NAME), db(dbManager.getDb()), playlistDao(db) {}

    void TearDown() override {
        db.exec("DELETE FROM playlists; VACUUM;");
        db.exec("UPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = 'playlists';");
    }

    static void TearDownTestSuite() {
        if (std::filesystem::exists(DB_NAME))
            std::filesystem::remove(DB_NAME);
    }

    static bool comparePlaylists(const PlaylistEntity& first, const PlaylistEntity& second) {
        return std::tie(
            first.id,
            first.name,
            first.cover_path
        ) == std::tie(
            second.id,
            second.name,
            second.cover_path
        );
    }
};

TEST_F(PlaylistDaoTest, InsertPlaylist) {
    PlaylistEntity playlist = {0, "Test pl", "/path/cover"};
    
    int playlistId = playlistDao.insertPlaylist(playlist);
    EXPECT_EQ(playlistId, 1);

    SQLite::Statement query(db, "SELECT * FROM playlists WHERE id = ?");
    query.bind(1, playlistId);
    query.executeStep();
    EXPECT_EQ(query.getColumn(0).getInt(), 1);
    EXPECT_EQ(query.getColumn(1).getString(), playlist.name);
    EXPECT_EQ(query.getColumn(2).getString(), playlist.cover_path);



    playlistId = playlistDao.insertPlaylist(playlist);
    EXPECT_EQ(playlistId, 2);

    query.reset();
    query.bind(1, playlistId);
    query.executeStep();
    EXPECT_EQ(query.getColumn(0).getInt(), 2);
    EXPECT_EQ(query.getColumn(1).getString(), playlist.name);
    EXPECT_EQ(query.getColumn(2).getString(), playlist.cover_path);



    playlist.cover_path = "";
    playlistId = playlistDao.insertPlaylist(playlist);
    EXPECT_EQ(playlistId, 3);

    query.reset();
    query.bind(1, playlistId);
    query.executeStep();
    EXPECT_EQ(query.getColumn(0).getInt(), 3);
    EXPECT_EQ(query.getColumn(1).getString(), playlist.name);
    EXPECT_TRUE(query.isColumnNull(2));



    playlist.name = "name2";
    playlist.cover_path = "path2";
    playlistId = playlistDao.insertPlaylist(playlist);
    EXPECT_EQ(playlistId, 4);

    query.reset();
    query.bind(1, playlistId);
    query.executeStep();
    EXPECT_EQ(query.getColumn(0).getInt(), 4);
    EXPECT_EQ(query.getColumn(1).getString(), playlist.name);
    EXPECT_EQ(query.getColumn(2).getString(), playlist.cover_path);
}
TEST_F(PlaylistDaoTest, DeletePlaylist) {
    EXPECT_FALSE(playlistDao.deletePlaylist(1));
    
    int playlistId = playlistDao.insertPlaylist({0, "name", ""});
    EXPECT_TRUE(playlistDao.deletePlaylist(playlistId));
}

TEST_F(PlaylistDaoTest, GetPlaylistById) {
    PlaylistEntity playlist = {0, "Test pl", "/path/cover"};

    int playlistId = playlistDao.insertPlaylist(playlist);
    playlist.id = playlistId;

    PlaylistEntity retrievedPlaylist = playlistDao.getPlaylistById(playlistId);
    EXPECT_TRUE(comparePlaylists(retrievedPlaylist, playlist));

    ASSERT_THROW(playlistDao.getPlaylistById(0), PlaylistNotFoundException);
    ASSERT_THROW(playlistDao.getPlaylistById(2), PlaylistNotFoundException);
}
TEST_F(PlaylistDaoTest, getAllPlaylists) {
    std::vector<PlaylistEntity> playlists = playlistDao.getAllPlaylists();
    EXPECT_EQ(playlists.size(), 0);

    PlaylistEntity playlist1 = {1, "Test pl", "/path/cover"};
    PlaylistEntity playlist2 = {2, "Test pl", "/path/cover"};
    PlaylistEntity playlist3 = {3, "Test pl2", ""};
    PlaylistEntity playlist4 = {4, "Test pl3", "/path/cover"};


    playlistDao.insertPlaylist(playlist1);
    playlistDao.insertPlaylist(playlist2);
    playlistDao.insertPlaylist(playlist3);
    playlistDao.insertPlaylist(playlist4);
    
    playlists = playlistDao.getAllPlaylists();
    
    ASSERT_EQ(playlists.size(), 4);
    EXPECT_TRUE(comparePlaylists(playlists[0], playlist1));
    EXPECT_TRUE(comparePlaylists(playlists[1], playlist2));
    EXPECT_TRUE(comparePlaylists(playlists[2], playlist3));
    EXPECT_TRUE(comparePlaylists(playlists[3], playlist4));
}
TEST_F(PlaylistDaoTest, GetPlaylistCoverPathById) {
    PlaylistEntity playlist = {0, "Test pl", "/path/cover"};
    int playlistId = playlistDao.insertPlaylist(playlist);

    std::string path = playlistDao.getPlaylistCoverPathById(playlistId);
    EXPECT_EQ(path, playlist.cover_path);

    db.exec("UPDATE playlists SET cover_path = NULL WHERE id = 1");

    path = playlistDao.getPlaylistCoverPathById(playlistId);
    EXPECT_EQ(path, "");

    ASSERT_THROW(playlistDao.getPlaylistCoverPathById(0), PlaylistNotFoundException);
    ASSERT_THROW(playlistDao.getPlaylistCoverPathById(2), PlaylistNotFoundException);
}

TEST_F(PlaylistDaoTest, UpdatePlaylistCoverPath) {
    PlaylistEntity playlist = {0, "Test pl", "/path/cover"};
    int playlistId = playlistDao.insertPlaylist(playlist);

    std::string newCoverPath = "/home/user/pics/cover.jpg";
    ASSERT_TRUE(playlistDao.updatePlaylistCoverPath(playlistId, newCoverPath));

    PlaylistEntity updatedPlaylist = playlistDao.getPlaylistById(playlistId);
    EXPECT_EQ(updatedPlaylist.cover_path, newCoverPath);
}

TEST_F(PlaylistDaoTest, UpdatePlaylistName) {
    PlaylistEntity playlist = {0, "Test pl", ""};
    int playlistId = playlistDao.insertPlaylist(playlist);

    std::string newName = "playlist";
    ASSERT_TRUE(playlistDao.updatePlaylistName(playlistId, newName));

    PlaylistEntity updatedPlaylist = playlistDao.getPlaylistById(playlistId);
    EXPECT_EQ(updatedPlaylist.name, newName);
}
