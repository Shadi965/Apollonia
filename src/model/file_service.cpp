#include <fstream>

#include "file_service.h"

FileService::FileService(std::string coversDir) {
    std::filesystem::path dir = coversDir;
    _pCoversDir = dir / "playlist_cover/";
    _aCoversDir = dir / "album_cover/";

    if (!std::filesystem::exists(_pCoversDir))
        std::filesystem::create_directories(_pCoversDir);
    if (!std::filesystem::exists(_aCoversDir))
        std::filesystem::create_directories(_aCoversDir);
}
FileService::~FileService() = default;


std::string FileService::savePlaylistCover(const std::string& name, const char* bytes, std::streamsize size) {
    std::filesystem::path filePath = _pCoversDir / name;

    std::ofstream outFile(filePath, std::ios::binary | std::ios::trunc);

    if (!outFile)
        return "";
    
    outFile.write(bytes, size);

    if (!outFile)
        return "";
    
    outFile.close();
    return std::filesystem::absolute(filePath);
}

std::string FileService::saveAlbumCover(const std::string& name, const char* bytes, std::streamsize size) {
    std::filesystem::path filePath = _aCoversDir / name;

    std::ofstream outFile(filePath, std::ios::binary | std::ios::trunc);

    if (!outFile)
        return "";
    
    outFile.write(bytes, size);

    if (!outFile)
        return "";
    
    outFile.close();
    return std::filesystem::absolute(filePath);
}

const std::string FileService::getFile(std::filesystem::path path) const {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return "";
    
    std::ostringstream strStream;
    strStream << file.rdbuf();
    return strStream.str();
}