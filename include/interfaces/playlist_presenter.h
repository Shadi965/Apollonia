#pragma once

#ifndef I_PLAYLIST_PRESENTER_H
#define I_PLAYLIST_PRESENTER_H

#include <vector>
#include <string>

struct Playlist {
    int id;
    std::string name;
    std::vector<int> songIds;
};

class IPlaylistPresenter {
public:
    virtual ~IPlaylistPresenter() = default;


    virtual const std::vector<Playlist> getAllPlaylists() const = 0;
    virtual const Playlist getPlaylist(int id) const = 0;

    virtual int createPlaylist(const std::string name) = 0;
    virtual bool renamePlaylist(int playlistId, const std::string newName) = 0;
    virtual bool deletePlaylist(int playlistId) = 0;

    virtual bool addSongToPlaylist(int playlistId, int songId) = 0;
    virtual bool removeSongFromPlaylist(int playlistId, int songId) = 0;

    // virtual img dloadPlaylistCover(int id) = 0;
    virtual bool uploadPlaylistCover(int id, const char* bytes, std::streamsize size) = 0;
};

#endif // I_PLAYLIST_PRESENTER_H
