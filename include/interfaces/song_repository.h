#pragma once

#ifndef I_SONG_REPOSITORY_H
#define I_SONG_REPOSITORY_H

#include <vector>

#include "entities.h"

class ISongRepository {
public:
    virtual ~ISongRepository() = default;

    virtual std::vector<SongEntity> getAllSongs() const = 0;
    virtual SongEntity getSongById(int id) const = 0;
    virtual std::vector<int> searchSongs(const std::string& query) const = 0;

    virtual SongMetaEntity getSongMetaById(int id) const = 0;
    virtual std::string getSongPathById(int id) const = 0;

    virtual std::vector<LyricLineEntity> getSongLyricsById(int id) const = 0;

};

#endif // I_SONG_REPOSITORY_H
