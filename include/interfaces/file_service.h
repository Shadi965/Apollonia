#pragma once

#ifndef I_FILE_SERVICE_H
#define I_FILE_SERVICE_H

#include <string>
#include <filesystem>

struct Chunk {
    std::string data;
    size_t size;
    size_t totalSize;
    std::string extension;
};

class IFileService {
public:
    virtual ~IFileService() = default;

    virtual std::string savePlaylistCover(const std::string& name, const char* bytes, std::streamsize size) = 0;
    virtual std::string saveAlbumCover(const std::string& name, const char* bytes, std::streamsize size) = 0;

    virtual const std::string getFile(std::filesystem::path path) const = 0;

    virtual const Chunk getChunk(std::filesystem::path path, std::streamsize start, std::streamsize end) const = 0;
    
    
};

#endif // I_FILE_SERVICE_H
