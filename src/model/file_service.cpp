#include <filesystem>
#include <fstream>

#include "file_service.h"

FileService::FileService(std::string coversDir) {
    if (!coversDir.empty() && coversDir.back() == '/')
        coversDir.pop_back();
    _pCoversDir = coversDir + "/playlist_cover/";
    _aCoversDir = coversDir + "/album_cover/";

    if (!std::filesystem::exists(_pCoversDir))
        std::filesystem::create_directories(_pCoversDir);
    if (!std::filesystem::exists(_aCoversDir))
        std::filesystem::create_directories(_aCoversDir);
}
FileService::~FileService() = default;


std::string FileService::savePlaylistCover(const std::string& name, const char* bytes, std::streamsize size) {
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

std::string FileService::saveAlbumCover(const std::string& name, const char* bytes, std::streamsize size) {
    std::string filePath = _aCoversDir + name;

    std::ofstream outFile(filePath, std::ios::binary | std::ios::trunc);

    if (!outFile)
        return "";
    
    outFile.write(bytes, size);

    if (!outFile)
        return "";
    
    outFile.close();
    return filePath;
}
