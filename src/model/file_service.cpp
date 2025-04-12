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

const Chunk FileService::getChunk(std::filesystem::path path, std::streamsize start, std::streamsize end) const {
    std::ifstream file(path, std::ios::binary);
    if (!file) return {"", 0, 0, ""};
    
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();

    if (start >= fileSize) return {"", 0, 0, ""};

    if (end >= fileSize) end = fileSize - 1;

    std::streamsize length = end - start + 1;
    std::string buffer(length, '\0');

    file.seekg(start, std::ios::beg);
    file.read(&buffer[0], length);
    std::streamsize bytesRead = file.gcount();

    buffer.resize(bytesRead);
    return {std::move(buffer), static_cast<size_t>(bytesRead), static_cast<size_t>(fileSize), path.extension().string()};
}
