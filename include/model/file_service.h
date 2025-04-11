#pragma once

#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include "interfaces/file_service.h"

class FileService : public IFileService {
public:
    FileService(std::string coversDir);
    ~FileService();

    std::string saveBytesToFile(const std::string& name, const char* bytes, std::streamsize size) override;

private:
    std::string _pCoversDir;
};

#endif // FILE_SERVICE_H
