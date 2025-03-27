#include "database.h"
#include <filesystem>
#include <iostream>
#include <set>
#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp>
#include <list>

extern "C" {
    #include <libavformat/avformat.h>
}

#define CREATE_TABLE_IF_NOT_EXISTS "CREATE TABLE IF NOT EXISTS songs (\
    id INTEGER PRIMARY KEY, title TEXT, artist TEXT, album_artist TEXT,\
    composer TEXT, album TEXT, track TEXT, disc TEXT, compilation TEXT,\
    date TEXT, copyright TEXT, genre TEXT, duration INTEGER, bitrate INTEGER,\
    channels INTEGER, sample_rate INTEGER, itunes_id INTEGER UNIQUE, file TEXT UNIQUE, last_modified INTEGER NOT NULL)"

#define GET_SONGS_PATHS "SELECT id, file FROM songs"
#define GET_FILES_LAST_WRITE_TIME "SELECT id, file, last_modified FROM songs"

#define GET_ALL_SONGS "SELECT * FROM songs"
#define GET_SONG_BY_ID "SELECT * FROM songs WHERE id = ?"
#define DELETE_SONG "DELETE FROM songs WHERE id = ?"
#define CHECK_SONG_EXISTS "SELECT COUNT(*) FROM songs WHERE file = ?"
#define INSERT_SONG "INSERT INTO songs (\
    title, artist, album_artist, composer,\
    album, track, disc, compilation, date, copyright,\
    genre, duration, bitrate, channels, sample_rate,\
    itunes_id, file, last_modified)\
    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"


int64_t last_write_time(const std::string& path);
std::map<std::string, int64_t> filesChangedDatesFromDir(const std::string& dir);
void findItunesId(Song& song, std::map<std::string, int>& itunesIds);
Song extractMetadata(AVFormatContext* formatCtx, std::pair<std::string, int64_t> fileChangedDate, std::map<std::string, int>& itunesIds);
std::map<std::string, int> loadJsonMetadata(const std::string& jsonPath);

Database::Database(const std::string& dbPath, const std::string& musicFolder, const std::string& jsonPath = "") : db(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    db.exec(CREATE_TABLE_IF_NOT_EXISTS);
    checkUpdates(musicFolder, jsonPath);
}

std::vector<Song> Database::getAllSongs() {
    std::vector<Song> songs;
    SQLite::Statement query(db, GET_ALL_SONGS);
    while (query.executeStep()) {
        songs.push_back({
            query.getColumn(0).getInt(), query.getColumn(1).getString(), query.getColumn(2).getString(),
            query.getColumn(3).getString(), query.getColumn(4).getString(), query.getColumn(5).getString(),
            query.getColumn(6).getString(), query.getColumn(7).getString(), query.getColumn(8).getString(),
            query.getColumn(9).getString(), query.getColumn(10).getString(), query.getColumn(11).getString(),
            query.getColumn(12).getInt(), query.getColumn(13).getInt(), query.getColumn(14).getInt(),
            query.getColumn(15).getInt(), query.getColumn(16).getInt(), query.getColumn(17).getString(),
            query.getColumn(17).getInt64()
        });
    }
    return songs;
}

Song Database::getSongById(int id) {
    SQLite::Statement query(db, GET_SONG_BY_ID);
    query.bind(1, id);
    if (query.executeStep())
        return {
            query.getColumn(0).getInt(), query.getColumn(1).getString(), query.getColumn(2).getString(),
            query.getColumn(3).getString(), query.getColumn(4).getString(), query.getColumn(5).getString(),
            query.getColumn(6).getString(), query.getColumn(7).getString(), query.getColumn(8).getString(),
            query.getColumn(9).getString(), query.getColumn(10).getString(), query.getColumn(11).getString(),
            query.getColumn(12).getInt(), query.getColumn(13).getInt(), query.getColumn(14).getInt(),
            query.getColumn(15).getInt(), query.getColumn(16).getInt(), query.getColumn(17).getString(),
            query.getColumn(17).getInt64()
        };
    Song song;
    song.id = -1;
    return song;
    
}

void Database::checkUpdates(const std::string& musicFolder, const std::string& jsonPath) {
    std::map<std::string, int64_t> filesInFolder = filesChangedDatesFromDir(musicFolder);

    filterFilesWithoutChanges(filesInFolder);

    std::map<std::string, int> itunesIds = loadJsonMetadata(jsonPath);

    AVFormatContext* formatCtx = nullptr;
    av_log_set_level(AV_LOG_ERROR);
    for (auto file : filesInFolder) {
        if (avformat_open_input(&formatCtx, file.first.c_str(), nullptr, nullptr) < 0)
            continue;
        if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
            avformat_close_input(&formatCtx);
            continue;
        }
        Song song = extractMetadata(formatCtx, file, itunesIds);
        insertSongIntoDb(song);
        avformat_close_input(&formatCtx);
    }
}



void Database::deleteSongsByIds(std::list<int>& ids) {
    SQLite::Statement stmt(db, DELETE_SONG);
    for (int id : ids) {
        stmt.bind(1, id);
        stmt.exec();
        stmt.reset();
    }
}

void Database::filterFilesWithoutChanges(std::map<std::string, int64_t>& filesInFolder) {
    SQLite::Statement query(db, GET_FILES_LAST_WRITE_TIME);
    std::list<int> toDeleteIds;
    std::map<std::string, int64_t>::iterator songInFolder;
    std::string filePath;
    while (query.executeStep()) {
        filePath = query.getColumn(1).getString();
        songInFolder = filesInFolder.find(filePath);
        if (songInFolder != filesInFolder.end()) {
            if (query.getColumn(2).getInt64() == songInFolder->second)
                filesInFolder.erase(songInFolder);
            else
                toDeleteIds.push_back(query.getColumn(0).getInt());
        } else
            toDeleteIds.push_back(query.getColumn(0).getInt());
    }

    deleteSongsByIds(toDeleteIds);
}

void Database::insertSongIntoDb(Song& song) {
    SQLite::Statement insert(db, INSERT_SONG);

    insert.bind(1, song.title);
    insert.bind(2, song.artist);
    insert.bind(3, song.albumArtist);
    insert.bind(4, song.composer);
    insert.bind(5, song.album);
    insert.bind(6, song.track);
    insert.bind(7, song.disc);
    insert.bind(8, song.compilation);
    insert.bind(9, song.date);
    insert.bind(10, song.copyright);
    insert.bind(11, song.genre);
    insert.bind(12, song.duration);
    insert.bind(13, song.bitrate);
    insert.bind(14, song.channels);
    insert.bind(15, song.sampleRate);
    insert.bind(17, song.file);
    insert.bind(18, song.lastModified);

    if (song.itunesId < 0)
        insert.bind(16);
    else
        insert.bind(16, song.itunesId);
    
    insert.exec();
}


int64_t last_write_time(const std::string& path) {
    auto ftime = std::filesystem::last_write_time(path);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    return std::chrono::system_clock::to_time_t(sctp);
}

std::map<std::string, int64_t> filesChangedDatesFromDir(const std::string& dir) {
    std::map<std::string, int64_t> filesInFolder;
    std::string filePath;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir))
        if (entry.is_regular_file()) {
            filePath = entry.path().string();
            filesInFolder.insert({filePath, last_write_time(filePath)});
        }
    return filesInFolder;
}

void findItunesId(Song& song, std::map<std::string, int>& itunesIds) {
    std::string key(song.artist + "|" + song.album + "|" + song.title);
    auto it = itunesIds.find(key);
    song.itunesId = it != itunesIds.end() ? it->second : -1;
}

Song extractMetadata(AVFormatContext* formatCtx, std::pair<std::string, int64_t> fileChangedDate, std::map<std::string, int>& itunesIds) {
    AVDictionaryEntry* tag = nullptr;
    Song song;
    if ((tag = av_dict_get(formatCtx->metadata, "title", nullptr, 0))) song.title = tag->value;
    if ((tag = av_dict_get(formatCtx->metadata, "artist", nullptr, 0))) song.artist = tag->value;
    if ((tag = av_dict_get(formatCtx->metadata, "album_artist", nullptr, 0))) song.albumArtist = tag->value;
    if ((tag = av_dict_get(formatCtx->metadata, "composer", nullptr, 0))) song.composer = tag->value;
    if ((tag = av_dict_get(formatCtx->metadata, "album", nullptr, 0))) song.album = tag->value;
    if ((tag = av_dict_get(formatCtx->metadata, "track", nullptr, 0))) song.track = tag->value;
    if ((tag = av_dict_get(formatCtx->metadata, "disc", nullptr, 0))) song.disc = tag->value;
    if ((tag = av_dict_get(formatCtx->metadata, "compilation", nullptr, 0))) song.compilation = tag->value;
    if ((tag = av_dict_get(formatCtx->metadata, "date", nullptr, 0))) song.date = tag->value;
    if ((tag = av_dict_get(formatCtx->metadata, "copyright", nullptr, 0))) song.copyright = tag->value;
    if ((tag = av_dict_get(formatCtx->metadata, "genre", nullptr, 0))) song.genre = tag->value;
    
    song.duration = formatCtx->duration / AV_TIME_BASE;
    song.bitrate = formatCtx->bit_rate / 1000;
    song.channels = formatCtx->streams[0]->codecpar->ch_layout.nb_channels;
    song.sampleRate = formatCtx->streams[0]->codecpar->sample_rate;

    song.file = fileChangedDate.first;
    song.lastModified = fileChangedDate.second;

    findItunesId(song, itunesIds);

    return song;
}

std::map<std::string, int> loadJsonMetadata(const std::string& jsonPath) {
    std::map<std::string, int> metadata;
    std::ifstream file(jsonPath);
    if (!file) return metadata;
    nlohmann::json jsonData;
    file >> jsonData;
    for (const auto& entry : jsonData) {
        std::string key = entry["artist"].get<std::string>() + "|" + entry["album"].get<std::string>() + "|" + entry["title"].get<std::string>();
        int id = entry.value("itunes_id", 0);
        metadata[key] = id;
    }
    return metadata;
}
