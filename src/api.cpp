#include "api.h"
#include <fstream>
#include <sstream>

crow::json::wvalue handleGetAllSongs(Database& db) {
    auto songs = db.getAllSongs();
    crow::json::wvalue songsJSON(songs.size());

    for (size_t i = 0; i < songs.size(); ++i) {
        songsJSON[i]["id"] = songs[i].id;
        songsJSON[i]["title"] = songs[i].title;
        songsJSON[i]["artist"] = songs[i].artist;
        songsJSON[i]["file"] = songs[i].file;
        songsJSON[i]["duration"] = songs[i].duration;
        songsJSON[i]["itunes_id"] = songs[i].itunesId;
    }

    return songsJSON;
}

crow::json::wvalue handleGetSongById(Database& db, int id) {
    auto song = db.getSongById(id);

    if (song.id == -1) {
        return crow::json::wvalue("Song not found");
    }

    crow::json::wvalue songJSON = {
        {"itunes_id", song.itunesId},
        {"duration", song.duration},
        {"file", song.file},
        {"artist", song.artist},
        {"title", song.title},
        {"id", song.id},
    };

    return songJSON;
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
    std::streampos end = fileSize - std::streamoff(1);
    if (!rangeHeader.empty()){
        size_t dashPos = rangeHeader.find('-');
        if (dashPos != std::string::npos){
            start = std::stoll(rangeHeader.substr(6, dashPos - 6));
            if (dashPos + 1 < rangeHeader.size())
                end = std::stoll(rangeHeader.substr(dashPos + 1));
        }

        if (end >= fileSize) end = fileSize - std::streamoff(1);
        if (start > end) start = end;
    }

    file.seekg(start);

    std::vector<char> buffer(end - start + 1);
    file.read(buffer.data(), buffer.size());

    res.body.assign(buffer.begin(), buffer.end());
    res.set_header("Content-Type", "audio/mpeg");
    res.set_header("Accept-Ranges", "bytes");
    res.set_header("Content-Range", "bytes " + std::to_string(start) + "-" + 
                   std::to_string(end) + "/" + std::to_string(fileSize));
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
    res.set_header("Content-Disposition", "attachment; filename=\"" + song.title + ".m4a\"");
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

    CROW_ROUTE(app, "/stream/<int>")
    ([&db](const crow::request& req, crow::response& res, int id){
        handleStream(req, res, db, id);
    });

    CROW_ROUTE(app, "/download/<int>")
    ([&db](const crow::request& req, crow::response& res, int id){
        handleDownload(req, res, db, id);
    });

    CROW_ROUTE(app, "/songs")([&db]() {
        return handleGetAllSongs(db);
    });

    CROW_ROUTE(app, "/songs/<int>")([&db](int id) {
        return handleGetSongById(db, id);
    });
}
