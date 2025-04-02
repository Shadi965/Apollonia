#ifndef SONG_REPOSITORY_H
#define SONG_REPOSITORY_H

#include <vector>

#include "file_service.h"
#include "songs.h"
#include "songs_meta.h"
#include "albums.h"
#include "lyrics.h"

class SongRepository {
public:
    SongRepository(SQLite::Database& db)
        : _songDao(db), _songMetaDao(db), _albumDao(db), _lyricsDao(db) {}


    Song getSongById(int songId);

    std::vector<Song> getSongs();

    SongMeta getSongMetaInfo(int songId);

    std::string getSongPath(int songId);

    Album getAlbumById(int albumId);

    std::vector<Album> getAlbums();

    void initDB(const std::string& musicDir, const std::string& lyricsJsonDir);


private:
    SongDao _songDao;
    SongMetaDao _songMetaDao;
    AlbumDao _albumDao;
    LyricsDao _lyricsDao;

    int addAlbumIfNotExist(const FileService& fileService);

    int addSong(const FileService& fileService, int albumId);

    void addSongMetaData(const FileService& fileService, int songId, const std::string& filePath);

    void takeDataFromFile(const std::string& filePath);

    void fixAlbums();

    void fillDbWithSongsFromDir(const std::string& directory);

    void fillLyricsFromJsonsFromDir(const std::string& directory);
};

#endif // SONG_REPOSITORY_H
