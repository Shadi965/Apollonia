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

private:
    std::string _pCoversDir;
    std::string _aCoversDir;
};

#endif // FILE_SERVICE_H
