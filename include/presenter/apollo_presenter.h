#pragma once

#ifndef SONG_PRESENTER_H
#define SONG_PRESENTER_H

#include "interfaces/song_presenter.h"
#include "interfaces/album_presenter.h"
#include "interfaces/playlist_presenter.h"
#include "interfaces/file_service.h"

#include "interfaces/song_repository.h"
#include "interfaces/album_repository.h"
#include "interfaces/playlist_repository.h"



class ApolloPresenter : public ISongPresenter, public IAlbumPresenter, public IPlaylistPresenter {
public:
    ApolloPresenter(ISongRepository& sr, IAlbumRepository& ar, IPlaylistRepository& pr,
                    IFileService& fs);
    ~ApolloPresenter() override;

    const std::vector<Song> getAllSongs() const override;
    const Song getSong(int id) const override;
    const std::vector<Song> getSongs(const std::vector<int> ids) const override;
    const std::vector<Song> searchSongs(const std::string& query) const override;

    const Lyrics getSongLyrics(int songId) const override;

    const FileData getFileChunk(int id, size_t start, size_t end = 0) const override;


    const std::vector<Album> getAllAlbums() const override;
    const Album getAlbum(int id) const override;
    const std::vector<Album> getAlbums(const std::vector<int> ids) const override;

    const FileData dloadAlbumCover(int id) const override;
    bool uploadAlbumCover(int id, const char* bytes, std::streamsize size, std::string fileExtension) override;

    const std::vector<int> getAlbumSongs(int albumId) const override;



    const std::vector<Playlist> getAllPlaylists() const override;
    const Playlist getPlaylist(int id) const override;

    int createPlaylist(const std::string name) override;
    bool renamePlaylist(int playlistId, const std::string newName) override;
    bool deletePlaylist(int playlistId) override;

    bool addSongToPlaylist(int playlistId, int songId, double position) override;
    bool removeSongFromPlaylist(int playlistId, int songId) override;
    bool updateSongPosition(int playlistId, int songId, double position) override;
    std::vector<int> addSongsToPlaylist(int playlistId, std::vector<std::pair<int, double>>& positionedSongs) override;

    const FileData dloadPlaylistCover(int id) const override;
    bool uploadPlaylistCover(int id, const char* bytes, std::streamsize size, std::string fileExtension) override;

    const std::vector<std::pair<int, double>> getPlaylistSongs(int playlistId) const override;
private:
    ISongRepository& _sr;
    IAlbumRepository& _ar;
    IPlaylistRepository& _pr;
    IFileService& _fs;

    static Song toSong(const SongEntity& se, const SongMetaEntity& sme);
    static Album toAlbum(const AlbumEntity& ae);
    static Playlist toPlaylist(const PlaylistEntity& pe);
};

#endif // SONG_PRESENTER_H
