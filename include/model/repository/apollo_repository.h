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
    ~ApolloRepository();

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

#endif // SONG_REPOSITORY_H
