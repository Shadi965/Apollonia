#pragma once

#ifndef I_SONG_PRESENTER_H
#define I_SONG_PRESENTER_H

#include <vector>
#include <string>

struct Song {
    int id;
    std::string title;
    std::string artist;
    std::string composer;
    int albumId;
    int track;
    int disc;
    std::string date;
    std::string copyright;
    std::string genre;
    int duration;
    int bitrate;
    int channels;
    int sampleRate;
};

struct Lyrics {
    int songId;
    std::vector<std::pair<int, std::string>> lrc;
};

class ISongPresenter {
public:
    virtual ~ISongPresenter() = default;


    virtual const std::vector<Song> getAllSongs() const = 0;
    virtual const Song getSong(int id) const = 0;

    virtual const Lyrics getSongLyrics(int songId) const = 0;

    // virtual songstr songStream(int id) = 0;
    // virtual song dloadSong(int id) = 0;
};

#endif // I_SONG_PRESENTER_H
