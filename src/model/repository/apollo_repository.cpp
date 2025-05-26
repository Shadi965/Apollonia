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

std::string ApolloRepository::getSongPathById(int id) const {
    return _songMetaDao.getFilePathBySongId(id);
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

std::string ApolloRepository::getAlbumCoverPath(int id) const {
    return _albumDao.getAlbumCoverPathById(id);
}
bool ApolloRepository::updateAlbumCoverPath(int id, const std::string& coverPath) {
    return _albumDao.updateAlbumCoverPath(id, coverPath);
}

std::vector<int> ApolloRepository::getAlbumSongs(int id) const {
    return _albumDao.getSongsFromAlbum(id);
}


std::vector<PlaylistEntity> ApolloRepository::getAllPlaylists() const {
    return _playlistDao.getAllPlaylists();
}
PlaylistEntity ApolloRepository::getPlaylistById(int id) const {
    return _playlistDao.getPlaylistById(id);
}

std::vector<std::pair<int, double>> ApolloRepository::getPlaylistSongs(int id) const {
    return _playlistSongDao.getSongsFromPlaylist(id);
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

bool ApolloRepository::addSongToPlaylist(int playlistId, int songId, double position) {
    bool result = _playlistSongDao.addSongToPlaylist(playlistId, songId, position);
    double fraction = position - (int)position;
    if (fraction < 1.0e-6)
        _playlistSongDao.spreadOutPositions(playlistId);
    return result;
}

bool ApolloRepository::removeSongFromPlaylist(int playlistId, int songId) {
    return _playlistSongDao.removeSongFromPlaylist(playlistId, songId);
}

bool ApolloRepository::updateSongPosition(int playlistId, int songId, double position) {
    return _playlistSongDao.updateSongPosition(playlistId, songId, position);
}

static bool needRecomposition(double position) {
    double scaled = position * 1000.0;
    return std::abs(scaled - (int)scaled) > 1.0e-3;
}

std::vector<PlaylistSongEntity> ApolloRepository::addSongsToPlaylist(std::vector<PlaylistSongEntity>& songs) {
    int playlistId = songs[0].playlist_id;
    std::vector<PlaylistSongEntity> notAdded;
    bool recomposition = false;
    for (auto& song : songs) {
        if (!recomposition && needRecomposition(song.position))
            recomposition = true;
        if (!_playlistSongDao.addSongToPlaylist(playlistId, song.song_id, song.position))
            notAdded.push_back(song);
    }
    if (recomposition)
        _playlistSongDao.spreadOutPositions(playlistId);
    return notAdded;
}


std::string ApolloRepository::getPlaylistCoverPath(int id) const {
    return _playlistDao.getPlaylistCoverPathById(id);
}
bool ApolloRepository::updatePlaylistCoverPath(int id, const std::string& coverPath) {
    return _playlistDao.updatePlaylistCoverPath(id, coverPath);
}
