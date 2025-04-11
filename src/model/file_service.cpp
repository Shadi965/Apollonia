#include <filesystem>
#include <fstream>

#include "file_service.h"

FileService::FileService(std::string coversDir) {
    if (!coversDir.empty() && coversDir.back() == '/')
        coversDir.pop_back();
    _pCoversDir = coversDir + "/playlist_cover/";

    if (!std::filesystem::exists(_pCoversDir))
        std::filesystem::create_directories(_pCoversDir);
}
FileService::~FileService() = default;


std::string FileService::saveBytesToFile(const std::string& name, const char* bytes, std::streamsize size) {
    std::string filePath = _pCoversDir + name;

    std::ofstream outFile(filePath, std::ios::binary | std::ios::trunc);

    if (!outFile)
        return "";
    
    outFile.write(bytes, size);

    if (!outFile)
        return "";
    
    outFile.close();
    return filePath;
}
