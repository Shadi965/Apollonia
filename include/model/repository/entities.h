#pragma once

#ifndef SONG_ENTITIES_H
#define SONG_ENTITIES_H

#include <string>

struct AlbumEntity {
    int id;
    std::string title;
    std::string artist;
    int track_count;
    int disc_count;
    bool compilation;
    std::string date;
    std::string copyright;
    std::string genre;
    std::string cover_path;
};

struct LyricLineEntity {
    int song_id;
    int time_ms;
    std::string line;
};

struct PlaylistSongEntity {
    int playlist_id;
    int song_id;
};

struct PlaylistEntity {
    int id;
    std::string name;
    std::string cover_path;
};

struct SongMetaEntity {
    int song_id;
    int duration;
    int bitrate;
    int channels;
    int sample_rate;
    int last_modified;
    std::string file;
};

struct SongEntity {
    int id;
    std::string title;
    std::string artist;
    std::string composer;
    int album_id;
    int track;
    int disc;
    std::string date;
    std::string copyright;
    std::string genre;
};

#endif // SONG_ENTITIES_H
