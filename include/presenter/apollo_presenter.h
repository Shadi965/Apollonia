#pragma once

#ifndef SONG_PRESENTER_H
#define SONG_PRESENTER_H

#include "interfaces/song_presenter.h"
#include "interfaces/album_presenter.h"
#include "interfaces/playlist_presenter.h"

#include "interfaces/song_repository.h"
#include "interfaces/album_repository.h"
#include "interfaces/playlist_repository.h"



class ApolloPresenter : public ISongPresenter, public IAlbumPresenter, public IPlaylistPresenter {
public:
    ApolloPresenter(ISongRepository& sr, IAlbumRepository& ar, IPlaylistRepository& pr);
    ~ApolloPresenter();

    const std::vector<Song> getAllSongs() const override;
    const Song getSong(int id) const override;

    const Lyrics getSongLyrics(int songId) const override;

    // virtual songstr songStream(int id) = 0;
    // virtual song dloadSong(int id) = 0;



    const std::vector<Album> getAllAlbums() const override;
    const Album getAlbum(int id) const override;

    // virtual img dloadAlbumCover(int id) = 0;



    const std::vector<Playlist> getAllPlaylists() const override;
    const Playlist getPlaylist(int id) const override;

    int createPlaylist(const std::string name) override;
    bool renamePlaylist(int playlistId, const std::string newName) override;
    bool deletePlaylist(int playlistId) override;

    bool addSongToPlaylist(int playlistId, int songId) override;
    bool removeSongFromPlaylist(int playlistId, int songId) override;

    // virtual img dloadPlaylistCover(int id) = 0;


private:
    ISongRepository& _sr;
    IAlbumRepository& _ar;
    IPlaylistRepository& _pr;

    static Song toSong(const SongEntity& se, const SongMetaEntity& sme);
    static Album toAlbum(const AlbumEntity& ae);
    static Playlist toPlaylist(const PlaylistEntity& pe, const std::vector<int>&& songs);
};

#endif // SONG_PRESENTER_H
