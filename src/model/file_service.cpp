#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <filesystem>

#include <nlohmann/json.hpp>

#include "file_service.h"


int FileService::inputOpen(const std::string& file) {
    int res = 0;
    av_log_set_level(AV_LOG_ERROR);
    res = avformat_open_input(&formatCtx, file.c_str(), nullptr, nullptr);
    if (res < 0)
        return res;

    res = avformat_find_stream_info(formatCtx, nullptr);
    if (res < 0)
        avformat_close_input(&formatCtx);

    return res;
}

void FileService::inputClose() {
    if (formatCtx != nullptr)
        avformat_close_input(&formatCtx);
}

bool FileService::isOpen() const {
    return formatCtx != nullptr;
}

bool FileService::isMedia() const {        
    for (unsigned int i = 0; i < formatCtx->nb_streams; ++i) {
        if (formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            return true;
        }
    }
    return false;
}




std::string FileService::getMetaValue(const char *key) const {
    AVDictionaryEntry* tag = av_dict_get(formatCtx->metadata, key, nullptr, 0);
    if (tag)
        return tag->value;
    return "";
}


std::string FileService::getSongTitle() const {
    return getMetaValue("title");
}

std::string FileService::getSongArtist() const {
    return getMetaValue("artist");
}

std::string FileService::getSongComposer() const {
    return getMetaValue("composer");
}

int FileService::getSongTrackNum() const {
    return std::atoi(getMetaValue("track").c_str());
}

int FileService::getSongDiscNum() const {
    return std::atoi(getMetaValue("disc").c_str());
}

std::string FileService::getSongDate() const {
    return getMetaValue("date");
}

std::string FileService::getSongCopyright() const {
    return getMetaValue("copyright");
}

std::string FileService::getSongGenre() const {
    return getMetaValue("genre");
}




std::string FileService::getSongAlbumName() const {
    return getMetaValue("album");
}

std::string FileService::getSongAlbumArtist() const {
    return getMetaValue("album_artist");
}

int FileService::getSongAlbumTrackCount() const {
    const char* trackCountPtr = getMetaValue("track").c_str();
    if (trackCountPtr = std::strchr(trackCountPtr, '/')) {
        ++trackCountPtr;
        return std::atoi(trackCountPtr);
    }
    return 0;
}

int FileService::getSongAlbumDiscCount() const {
    const char* discCountPtr = getMetaValue("disc").c_str();
    if (discCountPtr = std::strchr(discCountPtr, '/')) {
        ++discCountPtr;
        return std::atoi(discCountPtr);
    }
    return 0;
}

bool FileService::getIsSongAlbumCompilation() const {
    const char* compStr = getMetaValue("compilation").c_str();
    return std::atoi(compStr) != 0;
}




int64_t FileService::getSongDuration() const {
    return formatCtx->duration / AV_TIME_BASE;
}

int64_t FileService::getSongBitrate() const {
    return formatCtx->bit_rate / 1000;
}

int FileService::getSongChannelsCount() const {
    return formatCtx->streams[0]->codecpar->ch_layout.nb_channels;
}

int FileService::getSongSampleRate() const {
    return formatCtx->streams[0]->codecpar->sample_rate;
}

int64_t FileService::last_write_time(const std::string& path) {
    auto ftime = std::filesystem::last_write_time(path);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    return std::chrono::system_clock::to_time_t(sctp);
}


FileService::FileChunk FileService::getFileRange(const std::string& filePath, size_t start, size_t end) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Failed to open file: " + filePath);

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    if (start >= fileSize) {
        throw std::runtime_error("Requested range out of file bounds");
    }

    if (end >= fileSize) {
        end = fileSize - 1;
    }

    size_t readSize = end - start + 1;
    std::vector<uint8_t> buffer(readSize);

    file.seekg(start);
    file.read(reinterpret_cast<char*>(buffer.data()), readSize);

    return {buffer, fileSize, start, end};
}



const std::map<std::string, std::string>& FileService::scanDirWithLyricJsons(const std::string& directory) {
    lyricFiles.clear();
    std::string filePath;
    std::ifstream file;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.path().extension() == ".json")
        filePath = entry.path().string();

        file.open(filePath);
        if (!file.is_open()) continue;
        
        json jsonData;
        try {
            file >> jsonData;
            file.close();
        } catch (...) {
            file.close();
            continue;
        }

        lyricFiles.insert({getTrackFullName(jsonData), filePath});
    }
    return lyricFiles;
}

std::string FileService::getTrackFullName(const json& jsonData) const {
    std::string trackFullName;
    try {
        trackFullName = jsonData["data"]["track"]["artistName"].get<std::string>();
        trackFullName += jsonData["data"]["track"]["albumName"].get<std::string>();
        trackFullName += jsonData["data"]["track"]["name"].get<std::string>();
    } catch (...) {
        trackFullName = "";
    }

    return trackFullName;
}


std::vector<std::pair<int, std::string>> FileService::getSongLyrics(const std::string& songFullName) const {
    auto lyricPath = lyricFiles.find(songFullName);
    if (lyricPath == lyricFiles.end()) 
        return {};
    std::ifstream file(lyricPath->second);
    if (!file.is_open())
        return {};
    json jsonData;
    try {
        file >> jsonData;
    } catch (...) {
        return {};
    }
    return parseLyrics(jsonData);
}

std::vector<std::pair<int, std::string>> FileService::parseLyrics(const json& jsonData) const {
    std::vector<std::pair<int, std::string>> lyrics;
    try {
        const auto& data = jsonData["data"];
        const std::string lyricsKey = data.contains("trackStructureList") ? "trackStructureList" : "subtitle";
        
        if (lyricsKey == "trackStructureList") {
            for (const auto& section : data[lyricsKey]) {
                for (const auto& line : section["lines"]) {
                    lyrics.push_back({
                        static_cast<int>(line["time"]["total"].get<double>() * 1000),
                        line.contains("text") ? line["text"].get<std::string>() : ""
                    });
                }
            }
        } else {
            for (const auto& line : data[lyricsKey]) {
                lyrics.push_back({
                    static_cast<int>(line["time"]["total"].get<double>() * 1000),
                    line.contains("text") ? line["text"].get<std::string>() : ""
                });
            }
        }
    } catch (...) {
        // TODO: 
        std::cerr << "Error parsing lyrics." << std::endl;
    }
    return lyrics;
}
