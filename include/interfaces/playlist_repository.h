#pragma once

#ifndef I_PLAYLIST_REPOSITORY_H
#define I_PLAYLIST_REPOSITORY_H

#include <vector>

#include "entities.h"

class IPlaylistRepository {
public:
    virtual ~IPlaylistRepository() = default;

    virtual std::vector<PlaylistEntity> getAllPlaylists() const = 0;
    virtual PlaylistEntity getPlaylistById(int id) const = 0;

    virtual std::vector<int> getPlaylistSongs(int id) const = 0;

    virtual int createPlaylist(const std::string name) = 0;
    virtual bool renamePlaylist(int playlistId, const std::string newName) = 0;
    virtual bool deletePlaylist(int playlistId) = 0;

    virtual bool addSongToPlaylist(int playlistId, int songId) = 0;
    virtual bool removeSongFromPlaylist(int playlistId, int songId) = 0;
    
    virtual std::string getPlaylistCoverPath(int id) const = 0;
    virtual bool updatePlaylistCoverPath(int id, const std::string& coverPath) = 0;
};

#endif // I_PLAYLIST_REPOSITORY_H
