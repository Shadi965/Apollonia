#pragma once

#ifndef SONG_REPOSITORY_H
#define SONG_REPOSITORY_H

#include "interfaces/song_repository.h"
#include "interfaces/album_repository.h"
#include "interfaces/playlist_repository.h"

#include "dao/songs.h"
#include "dao/songs_meta.h"
#include "dao/lyrics.h"
#include "dao/albums.h"
#include "dao/playlists.h"
#include "dao/playlist_songs.h"

class Repository : public ISongRepository, public IAlbumRepository, public IPlaylistRepository {
public:
    Repository(SQLite::Database& db);
    ~Repository();

    std::vector<SongEntity> getAllSongs() const;
    SongEntity getSongById(int id) const;

    SongMetaEntity getSongMetaById(int id) const;

    std::vector<LyricLineEntity> getSongLyricsById(int id) const;



    std::vector<AlbumEntity> getAllAlbums() const;
    AlbumEntity getAlbumById(int id) const;



    std::vector<PlaylistEntity> getAllPlaylists() const;
    PlaylistEntity getPlaylistById(int id) const;

    std::vector<int> getPlaylistSongs(int id) const;

    int createPlaylist(const std::string name);
    bool renamePlaylist(int playlistId, const std::string newName);
    bool deletePlaylist(int playlistId);

    bool addSongToPlaylist(int playlistId, int songId);
    bool removeSongFromPlaylist(int playlistId, int songId);

private:
    SongDao _songDao;
    SongMetaDao _songMetaDao;
    LyricsDao _lyricsDao;
    AlbumDao _albumDao;
    PlaylistDao _playlistDao;
    PlaylistSongDao _playlistSongDao;
    
};

Repository::Repository(SQLite::Database& db) 
: _songDao(db), _songMetaDao(db), _lyricsDao(db), _albumDao(db), _playlistDao(db), _playlistSongDao(db) {}

Repository::~Repository() = default;

std::vector<SongEntity> Repository::getAllSongs() const {
    return _songDao.getAllSongs();
}
SongEntity Repository::getSongById(int id) const {
    return _songDao.getSongById(id);
}

SongMetaEntity Repository::getSongMetaById(int id) const {
    return _songMetaDao.getSongDataById(id);
}

std::vector<LyricLineEntity> Repository::getSongLyricsById(int id) const {
    return _lyricsDao.getLyricsForSong(id);
}



std::vector<AlbumEntity> Repository::getAllAlbums() const {
    return _albumDao.getAllAlbums();
}
AlbumEntity Repository::getAlbumById(int id) const {
    return _albumDao.getAlbumById(id);
}



std::vector<PlaylistEntity> Repository::getAllPlaylists() const {
    return _playlistDao.getAllPlaylists();
}
PlaylistEntity Repository::getPlaylistById(int id) const {
    return _playlistDao.getPlaylistById(id);
}

std::vector<int> Repository::getPlaylistSongs(int id) const {
    return _playlistSongDao.getSongsInPlaylist(id);
}

int Repository::createPlaylist(const std::string name) {
    return _playlistDao.insertPlaylist({0, name, ""});
}
bool Repository::renamePlaylist(int playlistId, const std::string newName) {
    return _playlistDao.updatePlaylistName(playlistId, newName);
}
bool Repository::deletePlaylist(int playlistId) {
    return _playlistDao.deletePlaylist(playlistId);
}

bool Repository::addSongToPlaylist(int playlistId, int songId) {
    return _playlistSongDao.addSongToPlaylist(playlistId, songId);
}
bool Repository::removeSongFromPlaylist(int playlistId, int songId) {
    return _playlistSongDao.removeSongFromPlaylist(playlistId, songId);
}

#endif // SONG_REPOSITORY_H
