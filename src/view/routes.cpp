#include "routes.h"

RoutesManager::RoutesManager(uint16_t port) {
    app.port(port).multithreaded();
}

void RoutesManager::runApp() {
    app.run();
}

void RoutesManager::regSongRoutes(const ISongPresenter& sp) {
    CROW_ROUTE(app, "/songs/")([&sp]() {
        return statusResponse(200, "success", songsJson(sp.getAllSongs()), "songs");
    });
    
    CROW_ROUTE(app, "/song/<int>")([&sp](int id) {
        try {
            return crow::response(200, songsJson(sp.getSong(id)));
        }
        catch(const SongNotFoundException& e) {
            return statusResponse(404, "fail", e.what());
        }
    });

    CROW_ROUTE(app, "/lyrics/<int>")([&sp](int id) {
        return statusResponse(200, "success", lyricsJson(sp.getSongLyrics(id)), "lyrics");
    });

    CROW_ROUTE(app, "/stream/<int>")([&sp](const crow::request& req, int id) {
        return crow::response(501);
    });

    CROW_ROUTE(app, "/download/<int>")([&sp](int id) {
        return crow::response(501);
    });
}

void RoutesManager::regAlbumRoutes(IAlbumPresenter& ap) {
    CROW_ROUTE(app, "/albums/")([&ap](){
        return statusResponse(200, "success", albumsJson(ap.getAllAlbums()), "albums");
    });

    CROW_ROUTE(app, "/album/<int>")([&ap](int id){
        try {
            return crow::response(200, albumsJson(ap.getAlbum(id)));
        }
        catch(const AlbumNotFoundException& e) {
            return statusResponse(404, "fail", e.what());
        }
    });

    CROW_ROUTE(app, "/album/cover/<int>").methods(crow::HTTPMethod::PUT)
    ([&ap](const crow::request& req, int id) {
        std::string ext = parseImgFileExt(req);
        if (ext.empty())
            return statusResponse(415, "fail", "Expected image type");
        
        if (ap.uploadAlbumCover(id, req.body.data(), req.body.size(), ext))
                return statusResponse(200, "success");
        
        return statusResponse(500);
    });

    CROW_ROUTE(app, "/album/cover/<int>").methods(crow::HTTPMethod::GET)
    ([&ap](int id) {
        auto [file, ext] = ap.dloadAlbumCover(id);
        if (file.empty() || ext.empty())
            return statusResponse(404, "fail", "Cover not found for this album");
        std::string type = imageTypeByExtension(ext);
        if (type.empty())
            return statusResponse(404, "fail", "Cover not found for this album");        
        
        return crow::response(200, type, file);
    });
}

void RoutesManager::regPlaylistRoutes(IPlaylistPresenter& pp) {
    CROW_ROUTE(app, "/playlists/")([&pp](){
        return statusResponse(200, "success", playlistsJson(pp.getAllPlaylists()), "playlists");
    });

    CROW_ROUTE(app, "/playlist/<int>")([&pp](int id){
        try {
            return statusResponse(200, "success", playlistsJson(pp.getPlaylist(id)), "");
    }
        catch(const PlaylistNotFoundException& e) {
            return statusResponse(404, "fail", e.what());
        }
    });

    CROW_ROUTE(app, "/playlist/").methods(crow::HTTPMethod::POST)
    ([&pp](const crow::request& req){ 
        std::string name = parseStrKey(req, "name");

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
        std::string newName = parseStrKey(req, "name");
        
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

    CROW_ROUTE(app, "/playlist/song/").methods(crow::HTTPMethod::POST)
    ([&pp](const crow::request& req) {
        if (req.get_header_value("Content-Type") == "application/json") {
            int playlistId = parseIntKey(req, "playlist_id");
            int songId = parseIntKey(req, "song_id");
            if (playlistId != 0 && songId != 0 && pp.addSongToPlaylist(playlistId, songId)) {
                return statusResponse(200, "success");
            }
        }
        return statusResponse(404, "fail");
    });

    CROW_ROUTE(app, "/playlist/song/").methods(crow::HTTPMethod::DELETE)
    ([&pp](const crow::request& req) {
        if (req.get_header_value("Content-Type") == "application/json") {
            int playlistId = parseIntKey(req, "playlist_id");
            int songId = parseIntKey(req, "song_id");
            if (playlistId != 0 && songId != 0 && pp.removeSongFromPlaylist(playlistId, songId)) {
                return statusResponse(200, "success");
            }
        }
        return statusResponse(404, "fail");
    });

    CROW_ROUTE(app, "/playlist/cover/<int>").methods(crow::HTTPMethod::PUT)
    ([&pp](const crow::request& req, int id) {
        std::string ext = parseImgFileExt(req);
        if (ext.empty())
            return statusResponse(415, "fail", "Expected image type");
        
        if (pp.uploadPlaylistCover(id, req.body.data(), req.body.size(), ext))
                return statusResponse(200, "success");
        
        return statusResponse(500);
    });

    CROW_ROUTE(app, "/playlist/cover/<int>").methods(crow::HTTPMethod::GET)
    ([&pp](int id) {
        auto [file, ext] = pp.dloadPlaylistCover(id);
        if (file.empty() || ext.empty())
            return statusResponse(404, "fail", "Cover not found for this playlist");
        std::string type = imageTypeByExtension(ext);
        if (type.empty())
            return statusResponse(404, "fail", "Cover not found for this playlist");        
        
        return crow::response(200, type, file);
    });
}


crow::response RoutesManager::statusResponse(int code, const std::string& status, const std::string& message) {
    crow::json::wvalue result;
    result["status"] = status;
    result["message"] = message;

    return crow::response(code, result);
}
crow::response RoutesManager::statusResponse(int code, const std::string& status, const crow::json::wvalue& json, const std::string& key) {
    crow::json::wvalue result;
    if (key.empty())
        result = crow::json::wvalue(json);
    else 
        result[key] = crow::json::wvalue(json);
    result["status"] = status;
    
    return crow::response(code, result);
}
crow::response RoutesManager::statusResponse(int code, const std::string& status) {
    crow::json::wvalue result;
    result["status"] = status;

    return crow::response(code, result);
}
crow::response RoutesManager::statusResponse(int code) {
    return crow::response(code);
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
        json["songs_ids"][i] = playlist.songIds[i];

    return json;
}
crow::json::wvalue RoutesManager::playlistsJson(const std::vector<Playlist>& playlists) {
    crow::json::wvalue json(playlists.size());

    for (size_t i = 0; i < playlists.size(); ++i)
        json[i] = crow::json::wvalue(playlistsJson(playlists[i]));
    
    return json;
}

std::string RoutesManager::parseStrKey(const crow::request& req, const std::string& key) {
    auto body = crow::json::load(req.body);
    if (!body.has(key)) {
        return "";
    }

    try {
        return body[key].s();
    }
    catch(const std::runtime_error& e) {
        return "";
    }
}

int RoutesManager::parseIntKey(const crow::request& req, const std::string& key) {
    auto body = crow::json::load(req.body);
    if (!body.has(key)) {
        return 0;
    }

    try {
        return body[key].i();
    }
    catch(const std::runtime_error& e) {
        return 0;
    }
}

std::string RoutesManager::parseImgFileExt(const crow::request& req) {
    std::string type = req.get_header_value("Content-Type");

    size_t pos = type.find('/');
    if (pos == std::string::npos) return "";

    std::string ext = type.substr(pos + 1);
    type = type.substr(0, pos);
    if (type != "image") return "";
    
    if (ext == "svg+xml") return "svg";
    if (ext == "x-icon") return "ico";
    
    return ext;
}

std::string RoutesManager::imageTypeByExtension(const std::string& ext) {
    static const std::unordered_map<std::string, std::string> imageTypes = {
        {".jpeg", "image/jpeg"},
        {".jpg",  "image/jpeg"},
        {".jfif", "image/jpeg"},
        {".png",  "image/png"},
        {".gif",  "image/gif"},
        {".bmp",  "image/bmp"},
        {".webp", "image/webp"},
        {".tiff", "image/tiff"},
        {".tif",  "image/tiff"},
        {".svg",  "image/svg+xml"},
        {".ico",  "image/x-icon"},
        {".heic", "image/heic"},
        {".heif", "image/heif"}
    };

    auto it = imageTypes.find(ext);
    if (it == imageTypes.end())
        return "";
    
    return it->second;
}
