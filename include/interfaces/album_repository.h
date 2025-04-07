#pragma once

#ifndef I_ALBUM_REPOSITORY_H
#define I_ALBUM_REPOSITORY_H

#include <vector>

#include "entities.h"

class IAlbumRepository {
public:
    virtual ~IAlbumRepository() = default;

    virtual std::vector<AlbumEntity> getAllAlbums() const = 0;
    virtual AlbumEntity getAlbumById(int id) const = 0;

    
};

#endif // I_ALBUM_REPOSITORY_H
