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
    

// class NoSongsException : public std::runtime_error {
// public:
//     NoSongsException() : std::runtime_error("No songs available in the database.") {}
// };

// class LyricsNotFoundException : public std::runtime_error {
// public:
//     const int songId;
//     LyricsNotFoundException(const int id)
//         : std::runtime_error("No lyrics found for song with ID " + std::to_string(id) + "."), songId(id) {}
// };
    


class AlbumNotFoundException : public std::runtime_error {
public:
    const int albumId;
    AlbumNotFoundException(const int id)
        : std::runtime_error("Album with ID " + std::to_string(id) + " not found."), albumId(id) {}
};
    

// class NoAlbumsException : public std::runtime_error {
// public:
//     NoAlbumsException() : std::runtime_error("No albums available in the database.") {}
// };

// class AlbumAlreadyExistsException : public std::runtime_error {
// public:
//     AlbumAlreadyExistsException(const std::string& title, const std::string& artist)
//         : std::runtime_error("Album with title '" + title + "' and artist '" + artist + "' already exists.") {}
// };



class PlaylistNotFoundException : public std::runtime_error {
public:
    const int playlistId;
    PlaylistNotFoundException(int id)
        : std::runtime_error("Playlist with ID " + std::to_string(id) + " not found."), playlistId(id) {}
};
    

// class NoPlaylistsException : public std::runtime_error {
// public:
//     NoPlaylistsException() 
//         : std::runtime_error("No playlists available in the database.") {}
// };

// class NoSongsInPlaylistException : public std::runtime_error {
// public:
//     const int playlistId;
//     NoSongsInPlaylistException(int id) 
//         : std::runtime_error("There are no songs in the playlist with ID " + std::to_string(id) + "."), playlistId(id) {}
// };
    

// class PlaylistUpdateException : public std::runtime_error {
// public:
//     PlaylistUpdateException(int id) 
//         : std::runtime_error("Failed to update playlist with ID " + std::to_string(id) + " because it does not exist.") {}
// };

// class PlaylistDeleteException : public std::runtime_error {
// public:
//     PlaylistDeleteException(int id) 
//         : std::runtime_error("Failed to delete playlist with ID " + std::to_string(id) + " because it does not exist.") {}
// };



#endif // REPOSITORY_EXCEPTIONS_H
