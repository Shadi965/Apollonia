#pragma once

#ifndef ROUTES_H
#define ROUTES_H

#include <crow.h>

#include "repository_exceptions.h"
#include "interfaces/song_presenter.h"
#include "interfaces/album_presenter.h"
#include "interfaces/playlist_presenter.h"

class RoutesManager {
private:
    crow::SimpleApp app;

public:
    RoutesManager(uint16_t port);
    void runApp();

    void regSongRoutes(const ISongPresenter& sp);
    void regAlbumRoutes(IAlbumPresenter& ap);
    void regPlaylistRoutes(IPlaylistPresenter& pp);

private:
    static crow::response statusResponse(int code);
    static crow::response statusResponse(int code, const std::string& status);
    static crow::response statusResponse(int code, const std::string& status, const std::string& message);
    static crow::response statusResponse(int code, const std::string& status, const crow::json::wvalue& json, const std::string& key);


    static crow::json::wvalue songsJson(const std::vector<Song>& songs);
    static crow::json::wvalue songsJson(const Song& song);

    static crow::json::wvalue lyricsJson(const Lyrics& lyrics);

    static crow::json::wvalue albumsJson(const std::vector<Album>& albums);
    static crow::json::wvalue albumsJson(const Album album);

    static crow::json::wvalue playlistsJson(const Playlist& playlist);
    static crow::json::wvalue playlistsJson(const std::vector<Playlist>& playlists);

    static std::string parseStrKey(const crow::request& req, const std::string& key);
    static int parseIntKey(const crow::request& req, const std::string& key);

    static std::string parseImgFileExt(const crow::request& req);
};

#endif // ROUTES_H
