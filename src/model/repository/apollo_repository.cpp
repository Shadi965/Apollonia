#include "apollo_repository.h"

ApolloRepository::ApolloRepository(SQLite::Database& db) 
: _songDao(db), _songMetaDao(db), _lyricsDao(db), _albumDao(db), _playlistDao(db), _playlistSongDao(db) {}

ApolloRepository::~ApolloRepository() = default;

std::vector<SongEntity> ApolloRepository::getAllSongs() const {
    return _songDao.getAllSongs();
}
SongEntity ApolloRepository::getSongById(int id) const {
    return _songDao.getSongById(id);
}

SongMetaEntity ApolloRepository::getSongMetaById(int id) const {
    return _songMetaDao.getSongDataById(id);
}

std::vector<LyricLineEntity> ApolloRepository::getSongLyricsById(int id) const {
    return _lyricsDao.getLyricsForSong(id);
}



std::vector<AlbumEntity> ApolloRepository::getAllAlbums() const {
    return _albumDao.getAllAlbums();
}
AlbumEntity ApolloRepository::getAlbumById(int id) const {
    return _albumDao.getAlbumById(id);
}



std::vector<PlaylistEntity> ApolloRepository::getAllPlaylists() const {
    return _playlistDao.getAllPlaylists();
}
PlaylistEntity ApolloRepository::getPlaylistById(int id) const {
    return _playlistDao.getPlaylistById(id);
}

std::vector<int> ApolloRepository::getPlaylistSongs(int id) const {
    return _playlistSongDao.getSongsInPlaylist(id);
}

int ApolloRepository::createPlaylist(const std::string name) {
    return _playlistDao.insertPlaylist({0, name, ""});
}
bool ApolloRepository::renamePlaylist(int playlistId, const std::string newName) {
    return _playlistDao.updatePlaylistName(playlistId, newName);
}
bool ApolloRepository::deletePlaylist(int playlistId) {
    return _playlistDao.deletePlaylist(playlistId);
}

bool ApolloRepository::addSongToPlaylist(int playlistId, int songId) {
    return _playlistSongDao.addSongToPlaylist(playlistId, songId);
}
bool ApolloRepository::removeSongFromPlaylist(int playlistId, int songId) {
    return _playlistSongDao.removeSongFromPlaylist(playlistId, songId);
}
