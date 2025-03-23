#include "database.h"
#include <filesystem>
#include <iostream>
#include <set>
#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp>
#include <libavformat/avformat.h>
#include <list>

#define CREATE_TABLE_IF_NOT_EXISTS "CREATE TABLE IF NOT EXISTS songs (\
    id INTEGER PRIMARY KEY, title TEXT, artist TEXT, albumArtist TEXT,\
    composer TEXT, album TEXT, track INTEGER, disc INTEGER, compilation BOOLEAN,\
    date TEXT, copyright TEXT, genre TEXT, duration INTEGER, bitrate INTEGER,\
    channels INTEGER, sampleRate INTEGER, file TEXT UNIQUE, isrc TEXT)"

#define GET_SONGS_PATHS "SELECT id, file FROM songs"

#define GET_ALL_SONGS "SELECT * FROM songs"
#define GET_SONG_BY_ID "SELECT * FROM songs WHERE id = ?"
#define DELETE_SONG "DELETE FROM songs WHERE file = ?"
#define CHECK_SONG_EXISTS "SELECT COUNT(*) FROM songs WHERE file = ?"
#define INSERT_SONG "INSERT INTO songs (id, title, artist, albumArtist, composer, album, track, disc, compilation, date, copyright, genre, duration, bitrate, channels, sampleRate, file, isrc) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"

Database::Database(const std::string& dbPath) : db(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    db.exec(CREATE_TABLE_IF_NOT_EXISTS);
}

// std::unordered_map<std::string, std::pair<int, std::string>> Database::loadJsonMetadata(const std::string& jsonPath) {
//     std::unordered_map<std::string, std::pair<int, std::string>> metadata;
//     std::ifstream file(jsonPath);
//     if (!file) return metadata;
//     nlohmann::json jsonData;
//     file >> jsonData;
//     for (const auto& entry : jsonData) {
//         std::string key = entry["artist"].get<std::string>() + "|" + entry["album"].get<std::string>() + "|" + entry["title"].get<std::string>();
//         int id = entry.value("id", 0);
//         std::string isrc = entry.value("isrc", "");
//         metadata[key] = {id, isrc};
//     }
//     return metadata;
// }

// std::vector<Song> Database::getAllSongs() {
//     std::vector<Song> songs;
//     SQLite::Statement query(db, GET_ALL_SONGS);
//     while (query.executeStep()) {
//         songs.push_back({
//             query.getColumn(0).getInt(), query.getColumn(1).getString(), query.getColumn(2).getString(),
//             query.getColumn(3).getString(), query.getColumn(4).getString(), query.getColumn(5).getString(),
//             query.getColumn(6).getString(), query.getColumn(7).getString(), query.getColumn(8).getString(),
//             query.getColumn(9).getString(), query.getColumn(10).getString(), query.getColumn(11).getString(),
//             query.getColumn(12).getInt(), query.getColumn(13).getInt(), query.getColumn(14).getInt(),
//             query.getColumn(15).getInt(), query.getColumn(16).getString(), query.getColumn(17).getString()
//         });
//     }
//     return songs;
// }

void Database::updateDatabase(const std::string& musicFolder, const std::string& jsonPath) {
    std::map<std::string, int> filesInFolder;
    AVFormatContext* formatCtx = nullptr;
    AVDictionaryEntry* tag = nullptr;
    std::string filePath;
    int idFromFile = -1;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(musicFolder)) {
        if (entry.is_regular_file()) {
            filePath = entry.path().string();
            if (avformat_open_input(&formatCtx, filePath.c_str(), nullptr, nullptr) < 0)
                continue;
            if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
                avformat_close_input(&formatCtx);
                continue;
            }
            idFromFile = -1;
            if ((tag = av_dict_get(formatCtx->metadata, "id", nullptr, 0))) idFromFile = atoi(tag->value);
            avformat_close_input(&formatCtx);
            filesInFolder.insert({filePath, idFromFile});
        }
    }
    
    SQLite::Statement query(db, GET_SONGS_PATHS);
    std::list<int> toDeleteList;
    std::map<std::string, int>::iterator songInBase()
    int id = -1;
    while (query.executeStep()) {
        filePath = query.getColumn(1).getString();
        if (filesInFolder.find(filePath) != filesInFolder.end()) {
            id = query.getColumn(0).getInt();
            if ()
        }
        
    }

}

void Database::updateDatabase(const std::string& musicFolder, const std::string& jsonPath) {
    std::set<std::string> filesInFolder;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(musicFolder))
        if (entry.is_regular_file())
            filesInFolder.insert(entry.path().string());

    std::unordered_map<std::string, std::pair<int, std::string>> jsonMetadata = loadJsonMetadata(jsonPath);
    SQLite::Statement insert(db, INSERT_SONG);
    for (const auto& filePath : filesInFolder) {
        AVFormatContext* formatCtx = nullptr;
        if (avformat_open_input(&formatCtx, filePath.c_str(), nullptr, nullptr) < 0)
            continue;
        if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
            avformat_close_input(&formatCtx);
            continue;
        }
        
        AVDictionaryEntry* tag = nullptr;
        Song song = Song::emptySong();
        song.file = filePath;
        if ((tag = av_dict_get(formatCtx->metadata, "title", nullptr, 0))) song.title = tag->value;
        if ((tag = av_dict_get(formatCtx->metadata, "artist", nullptr, 0))) song.artist = tag->value;
        if ((tag = av_dict_get(formatCtx->metadata, "album", nullptr, 0))) song.album = tag->value;
        
        std::string key = song.artist + "|" + song.album + "|" + song.title;
        auto it = jsonMetadata.find(key);
        song.id = (it != jsonMetadata.end() && it->second.first > 0) ? it->second.first : getNextAvailableId();
        song.isrc = (it != jsonMetadata.end()) ? it->second.second : "";

        if ((tag = av_dict_get(formatCtx->metadata, "album", nullptr, 0))) song.albumArtist = tag->value;
        if ((tag = av_dict_get(formatCtx->metadata, "album", nullptr, 0))) song.composer = tag->value;
        if ((tag = av_dict_get(formatCtx->metadata, "album", nullptr, 0))) song.track = tag->value;
        if ((tag = av_dict_get(formatCtx->metadata, "album", nullptr, 0))) song.disc = tag->value;
        if ((tag = av_dict_get(formatCtx->metadata, "album", nullptr, 0))) song.compilation = tag->value;
        if ((tag = av_dict_get(formatCtx->metadata, "album", nullptr, 0))) song.date = tag->value;
        if ((tag = av_dict_get(formatCtx->metadata, "album", nullptr, 0))) song.copyright = tag->value;
        if ((tag = av_dict_get(formatCtx->metadata, "album", nullptr, 0))) song.genre = tag->value;

        
        song.duration = formatCtx->duration / AV_TIME_BASE;
        song.bitrate = formatCtx->bit_rate / 1000;
        song.channels = formatCtx->streams[0]->codecpar->channels;
        song.sampleRate = formatCtx->streams[0]->codecpar->sample_rate;
        avformat_close_input(&formatCtx);
        
        insert.bind(1, song.id);
        insert.bind(2, title);
        insert.bind(3, artist);
        insert.bind(4, album);
        insert.bind(5, "Unknown");
        insert.bind(6, album);
        insert.bind(7, 0);
        insert.bind(8, 0);
        insert.bind(9, 0);
        insert.bind(10, "Unknown");
        insert.bind(11, "Unknown");
        insert.bind(12, "Unknown");
        insert.bind(13, duration);
        insert.bind(14, bitrate);
        insert.bind(15, channels);
        insert.bind(16, sampleRate);
        insert.bind(17, filePath);
        insert.bind(18, isrc);
        insert.exec();
        insert.reset();
    }
}

int Database::getNextAvailableId() {
    SQLite::Statement query(db, "SELECT MIN(id + 1) FROM songs WHERE (id + 1) NOT IN (SELECT id FROM songs)");
    return query.executeStep() ? query.getColumn(0).getInt() : 1;
}


