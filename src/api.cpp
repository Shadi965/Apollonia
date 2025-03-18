#include "api.h"
#include <nlohmann/json.hpp>
#include <fstream>

void setupRoutes(crow::SimpleApp& app, Database& db) {
    using json = nlohmann::json;

    CROW_ROUTE(app, "/stream/<int>")
    ([&db](const crow::request& req, crow::response& res, int id){
        Song song = db.getSongById(id);
        if (song.id == -1) {
            res.code = 404;
            res.end("Song not found");
            return;
        }
        res.set_header("Content-Type", "audio/mpeg");
        std::ifstream file(song.file, std::ios::binary);
        if (!file) {
            res.code = 500;
            res.end("Error opening file");
            return;
        }
        res.body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        res.end();
    });

    CROW_ROUTE(app, "/download/<int>")
    ([&db](const crow::request& req, crow::response& res, int id){
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
    });

    CROW_ROUTE(app, "/songs").methods(crow::HTTPMethod::GET)
    ([&db]() {
        auto songs = db.getAllSongs();
        json response;
        for (const auto& song : songs) {
            response["songs"].push_back({
                {"id", song.id},
                {"title", song.title},
                {"artist", song.artist},
                {"file", song.file},
                {"duration", song.duration}
            });
        }
        return crow::response(response.dump());
    });

    CROW_ROUTE(app, "/songs/<int>").methods(crow::HTTPMethod::GET)
    ([&db](int id) {
        auto song = db.getSongById(id);
        if (song.id == -1)
            return crow::response(404);
        json response = {
            {"id", song.id},
            {"title", song.title},
            {"artist", song.artist},
            {"file", song.file},
            {"duration", song.duration}
        };
        return crow::response(response.dump());
    });
}
