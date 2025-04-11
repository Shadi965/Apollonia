#pragma once

#ifndef I_FILE_SERVICE_H
#define I_FILE_SERVICE_H

#include <string>

class IFileService {
public:
    virtual ~IFileService() = default;

    virtual std::string saveBytesToFile(const std::string& name, const char* bytes, std::streamsize size) = 0;
    
};

#endif // I_FILE_SERVICE_H
