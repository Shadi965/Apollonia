#pragma once

#ifndef I_ALBUM_PRESENTER_H
#define I_ALBUM_PRESENTER_H

#include <vector>
#include <string>

struct Album {
    int id;
    std::string title;
    std::string artist;
    int trackCount;
    int discCount;
    bool compilation;
    std::string date;
    std::string copyright;
    std::string genre;
};

class IAlbumPresenter {
public:
    virtual ~IAlbumPresenter() = default;

    
    virtual const std::vector<Album> getAllAlbums() const = 0;
    virtual const Album getAlbum(int id) const = 0;

    virtual const std::pair<std::string, std::string> dloadAlbumCover(int id) const = 0;
    virtual bool uploadAlbumCover(int id, const char* bytes, std::streamsize size, std::string fileExtension) = 0;
    // virtual const std::string getAlbumCoverFileName(int id) const = 0;

};

#endif // I_ALBUM_PRESENTER_H
