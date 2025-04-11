#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include <string>
#include <fstream>
#include <vector>
#include <map>

extern "C" {
#include <libavformat/avformat.h>
}

#include <nlohmann/json.hpp>


class FileService {
private:
    using json = nlohmann::json;

    AVFormatContext* formatCtx = nullptr;
    std::map<std::string, std::string> lyricFiles;
public:

    FileService() {}

    FileService(const std::string& file) {
        inputOpen(file);
    }

    ~FileService() {
        inputClose();
    }

    int inputOpen(const std::string& file);

    void inputClose();

    bool isOpen() const;

    bool isMedia() const;


    std::string getSongTitle() const;

    std::string getSongArtist() const;

    std::string getSongComposer() const;

    int getSongTrackNum() const;

    int getSongDiscNum() const;

    std::string getSongDate() const;

    std::string getSongCopyright() const;

    std::string getSongGenre() const;


    std::string getSongAlbumName() const;

    std::string getSongAlbumArtist() const;

    int getSongAlbumTrackCount() const;

    int getSongAlbumDiscCount() const;

    bool getIsSongAlbumCompilation() const;


    int64_t getSongDuration() const;

    int64_t getSongBitrate() const;

    int getSongChannelsCount() const;

    int getSongSampleRate() const;

    const std::map<std::string, std::string>& scanDirWithLyricJsons(const std::string& directory);

    std::vector<std::pair<int, std::string>> getSongLyrics(const std::string& songFullName) const;

    std::vector<std::pair<int, std::string>> getSongLyrics( const std::string& artistName,
                                                            const std::string& albumName,
                                                            const std::string& title) const {
        return getSongLyrics(artistName + albumName + title);
    }


    static int64_t last_write_time(const std::string& path);


    struct FileChunk {
        std::vector<uint8_t> data;
        size_t fullSize;
        size_t start;
        size_t end;
    };

    static FileChunk getFileRange(const std::string& filePath, size_t start, size_t end);

private:

std::string getTrackFullName(const json& jsonData) const;

std::vector<std::pair<int, std::string>> parseLyrics(const json& jsonData) const;

std::string getMetaValue(const char *key) const;

};

#endif // FILE_SERVICE_H
