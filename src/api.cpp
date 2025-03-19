#include "api.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>

void handleGetAllSongs(const crow::request& req, crow::response& res, Database& db) {
    auto songs = db.getAllSongs();
    nlohmann::json jsonSongs = nlohmann::json::array();

    for (const auto& song : songs) {
        jsonSongs.push_back({
            {"id", song.id},
            {"title", song.title},
            {"artist", song.artist},
            {"file", song.file},
            {"duration", song.duration}
        });
    }

    res.set_header("Content-Type", "application/json");
    res.write(jsonSongs.dump());
    res.end();
}

void handleGetSongById(const crow::request& req, crow::response& res, Database& db, int id) {
    auto song = db.getSongById(id);
    if (song.id == -1) {
        res.code = 404;
        res.end("Song not found");
        return;
    }

    nlohmann::json jsonSong = {
        {"id", song.id},
        {"title", song.title},
        {"artist", song.artist},
        {"file", song.file},
        {"duration", song.duration}
    };

    res.set_header("Content-Type", "application/json");
    res.write(jsonSong.dump());
    res.end();
}

void handleStream(const crow::request& req, crow::response& res, Database& db, int id) {
    Song song = db.getSongById(id);
    if (song.id == -1) {
        res.code = 404;
        res.end("Song not found");
        return;
    }
    
    std::ifstream file(song.file, std::ios::binary);
    if (!file) {
        res.code = 500;
        res.end("Error opening file");
        return;
    }

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string rangeHeader = req.get_header_value("Range");
    std::streampos start = 0;
    if (!rangeHeader.empty()){
        std::istringstream rangeStream(rangeHeader.substr(6));
        start = std::stoll(rangeHeader.substr(6));
    }

    file.seekg(start);

    std::ostringstream buffer;
    buffer << file.rdbuf();
    res.body = buffer.str();

    res.set_header("Content-Type", "audio/mpeg");
    res.set_header("Accept-Ranges", "bytes");
    res.set_header("Content-Range", "bytes " + std::to_string(start) + "-" + 
                   std::to_string(static_cast<std::streamoff>(fileSize) - 1) + "/" + 
                   std::to_string(static_cast<std::streamoff>(fileSize)));
    res.code = 206;
    res.end();
}

void handleDownload(const crow::request& req, crow::response& res, Database& db, int id) {
    Song song = db.getSongById(id);
    if (song.id == -1) {
        res.code = 404;
        res.end("Song not found");
        return;
    }
    res.set_header("Content-Disposition", "attachment; filename=\"" + song.title + ".mp3\"");
    std::ifstream file(song.file, std::ios::binary);
    if (!file) {
        res.code = 500;
        res.end("Error opening file");
        return;
    }
    res.body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    res.end();
}

void setupRoutes(crow::SimpleApp& app, Database& db) {
    using json = nlohmann::json;

    CROW_ROUTE(app, "/stream/<int>")
    ([&db](const crow::request& req, crow::response& res, int id){
        handleStream(req, res, db, id);
    });

    CROW_ROUTE(app, "/download/<int>")
    ([&db](const crow::request& req, crow::response& res, int id){
        handleDownload(req, res, db, id);
    });

    CROW_ROUTE(app, "/songs")([&db](const crow::request& req, crow::response& res) {
        handleGetAllSongs(req, res, db);
    });

    CROW_ROUTE(app, "/songs/<int>")([&db](const crow::request& req, crow::response& res, int id) {
        handleGetSongById(req, res, db, id);
    });
}
