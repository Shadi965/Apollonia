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

class ApolloRepository : public ISongRepository, public IAlbumRepository, public IPlaylistRepository {
public:
    ApolloRepository(SQLite::Database& db);
    ~ApolloRepository() override;

    std::vector<SongEntity> getAllSongs() const override;
    SongEntity getSongById(int id) const override;

    SongMetaEntity getSongMetaById(int id) const override;

    std::vector<LyricLineEntity> getSongLyricsById(int id) const override;



    std::vector<AlbumEntity> getAllAlbums() const override;
    AlbumEntity getAlbumById(int id) const override;

    std::string getAlbumCoverPath(int id) const override;
    bool updateAlbumCoverPath(int id, const std::string& coverPath) override;




    std::vector<PlaylistEntity> getAllPlaylists() const override;
    PlaylistEntity getPlaylistById(int id) const override;

    std::vector<int> getPlaylistSongs(int id) const override;

    int createPlaylist(const std::string name) override;
    bool renamePlaylist(int playlistId, const std::string newName) override;
    bool deletePlaylist(int playlistId) override;

    bool addSongToPlaylist(int playlistId, int songId) override;
    bool removeSongFromPlaylist(int playlistId, int songId) override;

    std::string getPlaylistCoverPath(int id) const override;
    bool updatePlaylistCoverPath(int id, const std::string& coverPath) override;

private:
    SongDao _songDao;
    SongMetaDao _songMetaDao;
    LyricsDao _lyricsDao;
    AlbumDao _albumDao;
    PlaylistDao _playlistDao;
    PlaylistSongDao _playlistSongDao;
    
};

#endif // SONG_REPOSITORY_H
