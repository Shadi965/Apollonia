#include "routes.h"

RoutesManager::RoutesManager(uint16_t port) {
    app.port(port).multithreaded();
}

void RoutesManager::runApp() {
    app.run();
}

void RoutesManager::regSongRoutes(const ISongPresenter& sp) {
    CROW_ROUTE(app, "/songs")([&sp]() {
        return songsJson(sp.getAllSongs());
    });
    
    CROW_ROUTE(app, "/song/<int>")([&sp](int id) {
        try {
            return crow::response(200, songsJson(sp.getSong(id)));
        }
        catch(const SongNotFoundException& e) {
            return statusResponse(404, "error", e.what());
        }
    });

    CROW_ROUTE(app, "/lyrics/<int>")([&sp](int id) {
        return lyricsJson(sp.getSongLyrics(id));
    });

    // CROW_ROUTE(app, "/stream/<int>")([&sp](const crow::request& req, int id) {
    //     return sp.streamSong(id, req);
    // });

    // CROW_ROUTE(app, "/download/<int>")([&sp](int id) {
    //     return sp.downloadSong(id);
    // });
}

void RoutesManager::regAlbumRoutes(const IAlbumPresenter& ap) {
    CROW_ROUTE(app, "/albums/")([&ap](){
        return albumsJson(ap.getAllAlbums());
    });

    CROW_ROUTE(app, "/album/<int>")([&ap](int id){
        try {
            return crow::response(200, albumsJson(ap.getAlbum(id)));
        }
        catch(const AlbumNotFoundException& e) {
            return statusResponse(404, "error", e.what());
        }
    });
}

void RoutesManager::regPlaylistRoutes(IPlaylistPresenter& pp) {
    CROW_ROUTE(app, "/playlists/")([&pp](){
        return playlistsJson(pp.getAllPlaylists());
    });

    CROW_ROUTE(app, "/playlist/<int>")([&pp](int id){
        try {
            return crow::response(200, playlistsJson(pp.getPlaylist(id)));
        }
        catch(const PlaylistNotFoundException& e) {
            return statusResponse(404, "error", e.what());
        }
    });

    CROW_ROUTE(app, "/playlist/").methods(crow::HTTPMethod::POST)
    ([&pp](const crow::request& req){ 
        std::string name = parseName(req);

        if (name.empty())
            return statusResponse(400, "fail", "Missing 'name' in request");
        
        int newId = pp.createPlaylist(name);

        crow::json::wvalue result;
        result["status"] = "success";
        result["id"] = newId;
        return crow::response(201, result);
    });

    CROW_ROUTE(app, "/playlist/<int>").methods(crow::HTTPMethod::PATCH)
    ([&pp](const crow::request& req, int id){
        std::string newName = parseName(req);
        
        if (newName.empty())
            return statusResponse(400, "fail", "Missing 'name' in request");
        
        if (!pp.renamePlaylist(id, newName))
            return statusResponse(404, "fail", "Playlist not found");

        crow::json::wvalue result;
        result["status"] = "success";

        return crow::response(200, result);
    });

    CROW_ROUTE(app, "/playlist/<int>").methods(crow::HTTPMethod::DELETE)
    ([&pp](int id) {
        if (pp.deletePlaylist(id))
            return statusResponse(200, "success", "Playlist deleted successfully");
         else
            return statusResponse(404, "fail", "Nothing was deleted");
    });
}


crow::response RoutesManager::statusResponse(int code, std::string status, std::string message) {
    crow::json::wvalue result;
    result["status"] = status;
    result["message"] = message;

    return crow::response(code, result);
}
crow::response RoutesManager::statusResponse(int code, std::string status) {
    crow::json::wvalue result;
    result["status"] = status;

    return crow::response(code, result);
}

crow::json::wvalue RoutesManager::songsJson(const std::vector<Song>& songs) {
    crow::json::wvalue json(songs.size());

    for (size_t i = 0; i < songs.size(); ++i) {
        json[i]["id"] = songs[i].id;
        json[i]["title"] = songs[i].title;
        json[i]["artist"] = songs[i].artist;
        json[i]["composer"] = songs[i].composer;
        json[i]["album_id"] = songs[i].albumId;
        json[i]["track"] = songs[i].track;
        json[i]["disc"] = songs[i].disc;
        json[i]["date"] = songs[i].date;
        json[i]["copyright"] = songs[i].copyright;
        json[i]["genre"] = songs[i].genre;
        json[i]["duration"] = songs[i].duration;
        json[i]["bitrate"] = songs[i].bitrate;
        json[i]["channels"] = songs[i].channels;
        json[i]["sample_rate"] = songs[i].sampleRate;
    }

    return json;
}
crow::json::wvalue RoutesManager::songsJson(const Song& song) {
    return songsJson(std::vector<Song>{song});
}

crow::json::wvalue RoutesManager::lyricsJson(const Lyrics& lyrics) {
    crow::json::wvalue json;

    json["song_id"] = lyrics.songId;
    for (size_t i = 0; i < lyrics.lrc.size(); ++i) {
        json["lyrics"][i]["time"] = lyrics.lrc[i].first;
        json["lyrics"][i]["text"] = lyrics.lrc[i].second;
    }
    return json;
}

crow::json::wvalue RoutesManager::albumsJson(const std::vector<Album>& albums) {
    crow::json::wvalue json(albums.size());

    for (size_t i = 0; i < albums.size(); ++i) {
        json[i]["id"] = albums[i].id;
        json[i]["title"] = albums[i].title;
        json[i]["artist"] = albums[i].artist;
        json[i]["track_count"] = albums[i].trackCount;
        json[i]["disc_count"] = albums[i].discCount;
        json[i]["compilation"] = albums[i].compilation;
        json[i]["date"] = albums[i].date;
        json[i]["copyright"] = albums[i].copyright;
        json[i]["genre"] = albums[i].genre;
    }

    return json;
}
crow::json::wvalue RoutesManager::albumsJson(const Album album) {
    return albumsJson(std::vector<Album>{album});
}

crow::json::wvalue RoutesManager::playlistsJson(const Playlist& playlist) {
    crow::json::wvalue json;

    json["id"] = playlist.id;
    json["name"] = playlist.name;

    for (size_t i = 0; i < playlist.songIds.size(); ++i)
        json["songs"][i] = playlist.songIds[i];

    return json;
}
crow::json::wvalue RoutesManager::playlistsJson(const std::vector<Playlist>& playlists) {
    crow::json::wvalue json;

    for (size_t i = 0; i < playlists.size(); ++i)
        json[i] = playlistsJson(playlists[i]);
    
    return json;
}

std::string RoutesManager::parseName(const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body.has("name")) {
        return "";
    }

    return body["name"].s();
}
