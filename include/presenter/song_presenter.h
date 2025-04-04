#ifndef SONG_PRESENTER_H
#define SONG_PRESENTER_H

#include <crow.h>

#include "song_repository.h"
#include "file_service.h"

class SongPresenter
{
private:
    SongRepository& _songRepos;
    FileService& _fileService;
public:
    SongPresenter(SongRepository& songRepository, FileService& fileService) 
    : _songRepos(songRepository), _fileService(fileService) {}

    crow::json::wvalue allSongsInfo() {
        auto songs = _songRepos.getSongs();
        crow::json::wvalue res(songs.size());

        for (size_t i = 0; i < songs.size(); ++i) {
            res[i]["id"] = songs[i].id;
            res[i]["title"] = songs[i].title;
            res[i]["artist"] = songs[i].artist;
            res[i]["composer"] = songs[i].composer;
            res[i]["album_id"] = songs[i].albumId;
            res[i]["track"] = songs[i].track;
            res[i]["disc"] = songs[i].disc;
            res[i]["date"] = songs[i].date;
            res[i]["copyright"] = songs[i].copyright;
            res[i]["genre"] = songs[i].genre;
        }
    
        return res;
    }

    crow::json::wvalue songInfo(int id) {
        Song song = _songRepos.getSongById(id);
    
        return {
            {"id", song.id},
            {"title", song.title},
            {"artist", song.artist},
            {"composer", song.composer},
            {"album_id", song.albumId},
            {"track", song.track},
            {"disc", song.disc},
            {"date", song.date},
            {"copyright", song.copyright},
            {"genre", song.genre}
        };
    }

    crow::json::wvalue songDuration(int id) {
        return {
            {"duration", _songRepos.getSongDuration(id)}
        };
    }

    crow::json::wvalue songsMeta() {
        auto songsMeta = _songRepos.getSongsMeta();
        crow::json::wvalue res(songsMeta.size());

        for (size_t i = 0; i < songsMeta.size(); ++i) {
            res[i]["song_id"] = songsMeta[i].songId;
            res[i]["duration"] = songsMeta[i].duration;
            res[i]["bitrate"] = songsMeta[i].bitrate;
            res[i]["channels"] = songsMeta[i].channels;
            res[i]["sample_rate"] = songsMeta[i].sampleRate;
        }
    
        return res;
    }

    crow::json::wvalue songLyric(int id) {
        std::vector<LyricLine> lyrics = _songRepos.getLyricsForSong(id);

        crow::json::wvalue res(lyrics.size());

        for (size_t i = 0; i < lyrics.size(); ++i) {
            res[i]["time"] = lyrics[i].timeMs;
            res[i]["text"] = lyrics[i].line;
        }
    
        return res;
    }
    

    crow::response downloadSong(int id) {
        Song song = _songRepos.getSongById(id);
        std::string filePath = _songRepos.getSongPath(id);
        
        // Открываем файл для бинарного чтения
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            return crow::response(500, "Error opening file");
        }
    
        // Определяем размер файла
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
    
        // Формируем заголовки
        crow::response res;
        res.set_header("Content-Disposition", "attachment; filename=\"" + song.title + ".m4a\"");
        res.set_header("Content-Type", "audio/x-m4a");
        res.set_header("Content-Length", std::to_string(fileSize));
    
        // Потоковая передача файла
        res.body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
        return res;
    }

    crow::response streamSong(int id, const crow::request& req) {
        std::string filePath = _songRepos.getSongPath(id);
        size_t fileSize = std::filesystem::file_size(filePath);
        const size_t chunkSize = 1048576; // 1MB
    
        // Парсим заголовок Range
        size_t start = 0, end = fileSize - 1;
        bool isPartial = false;
    
        auto it = req.headers.find("range");
        if (it != req.headers.end()) {
            std::string rangeHeader = it->second;
            if (sscanf(rangeHeader.c_str(), "bytes=%zu-%zu", &start, &end) >= 1) {
                if (it->second.find('-') == it->second.size() - 1) {
                    end = std::min(start + chunkSize - 1, fileSize - 1); // Ограничиваем чанк
                }
                isPartial = true;
            }
        }
    
        // Получаем кусок файла через FileService
        auto fileChunk = FileService::getFileRange(filePath, start, end);
    
        // Формируем HTTP-ответ
        crow::response res(isPartial ? 206 : 200); // 206 Partial Content, если Range есть
        res.set_header("Content-Type", "audio/x-m4a");
        res.set_header("Accept-Ranges", "bytes");
    
        if (isPartial) {
            res.set_header("Content-Range", "bytes " + std::to_string(fileChunk.start) + "-" + 
                                          std::to_string(fileChunk.end) + "/" + 
                                          std::to_string(fileChunk.fullSize));
        }
    
        res.set_header("Content-Length", std::to_string(fileChunk.data.size()));
    
        // Устанавливаем тело ответа (без копирования)
        res.body = std::string(reinterpret_cast<char*>(fileChunk.data.data()), fileChunk.data.size());
    
        return res;
    }


    crow::json::wvalue allPlaylists() {
        auto playlists = _songRepos.getAllPlaylists();
        crow::json::wvalue res(playlists.size());

        for (size_t i = 0; i < playlists.size(); ++i) {
            res[i]["id"] = playlists[i].id;
            res[i]["title"] = playlists[i].name;
        }
    
        return res;
    }

    void createPlaylist(const std::string& name) {
        _songRepos.newPlaylist(name);
    }

    void changePlaylistName(int id, const std::string& newName) {
        _songRepos.changePlaylistName(id, newName);
    }

    void deletePlaylist(int id) {
        _songRepos.removePlaylist(id);
    }

    crow::json::wvalue getAllSongsFromPlaylist(int id) {
        auto songs = _songRepos.getAllSongsFromPlaylist(id);
        crow::json::wvalue res(songs.size());

        for (size_t i = 0; i < songs.size(); ++i) {
            res[i]["id"] = songs[i];
        }
    
        return res;
    }

    void addSongToPlaylist(int playlistId, int songId) {
        _songRepos.addSongToPlaylist(playlistId, songId);
    }

    void removeSongFromPlaylist(int playlistId, int songId) {
        _songRepos.removeSongFromPlaylist(playlistId, songId);
    }
};

#endif // SONG_PRESENTER_H
