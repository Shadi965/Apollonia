#pragma once

#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include "interfaces/file_service.h"

class FileService : public IFileService {
public:
    FileService(std::string coversDir);
    ~FileService() override;

    std::string savePlaylistCover(const std::string& name, const char* bytes, std::streamsize size) override;
    std::string saveAlbumCover(const std::string& name, const char* bytes, std::streamsize size) override;

    const std::string getFile(std::filesystem::path path) const override;
    const Chunk getChunk(std::filesystem::path path, std::streamsize start, std::streamsize end) const override;

private:
    std::filesystem::path _pCoversDir;
    std::filesystem::path _aCoversDir;
};

#endif // FILE_SERVICE_H
