#pragma once

#ifndef REPOSITORY_EXCEPTIONS_H
#define REPOSITORY_EXCEPTIONS_H

#include <stdexcept>
#include <string>



class SongNotFoundException : public std::runtime_error {
public:
    const int songId;
    SongNotFoundException(const int id)
        : std::runtime_error("Song with ID " + std::to_string(id) + " not found."), songId(id) {}
};

class SongAlreadyExists : public std::runtime_error {
public:
    SongAlreadyExists() : std::runtime_error("A song with these parameters is already in the table") {}
};
    


class AlbumNotFoundException : public std::runtime_error {
public:
    const int albumId;
    AlbumNotFoundException(const int id)
        : std::runtime_error("Album with ID " + std::to_string(id) + " not found."), albumId(id) {}
};



class PlaylistNotFoundException : public std::runtime_error {
public:
    const int playlistId;
    PlaylistNotFoundException(int id)
        : std::runtime_error("Playlist with ID " + std::to_string(id) + " not found."), playlistId(id) {}
};



#endif // REPOSITORY_EXCEPTIONS_H
