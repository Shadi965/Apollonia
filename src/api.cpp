#include "api.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void setupRoutes(crow::SimpleApp& app, Database& db) {
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
