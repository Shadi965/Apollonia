#ifndef SONG_REPOSITORY_H
#define SONG_REPOSITORY_H

#include <vector>

#include "file_service.h"
#include "songs.h"
#include "songs_meta.h"
#include "albums.h"
#include "lyrics.h"

#include <playlists.h>
#include <playlist_songs.h>

class SongRepository {
public:
    SongRepository(SQLite::Database& db)
        : _songDao(db), _songMetaDao(db), _albumDao(db), _lyricsDao(db), _playlistDao(db), _playlistSongDao(db) {

        }


    Song getSongById(int songId);

    std::vector<Song> getSongs();

    std::vector<SongMeta> getSongsMeta();

    SongMeta getSongMetaInfo(int songId);

    int getSongDuration(int songId);

    std::string getSongPath(int songId);

    Album getAlbumById(int albumId);

    std::vector<Album> getAlbums();


    std::vector<LyricLine> getLyricsForSong(int songId);

    void initDB(const std::string& musicDir, const std::string& lyricsJsonDir);

    std::vector<Playlist> getAllPlaylists();

    int newPlaylist(const std::string& name);

    bool changePlaylistName(int id, const std::string& newName);

    void removePlaylist(int id);

    void addSongToPlaylist(int playlistId, int songId);

    void removeSongFromPlaylist(int playlistId, int songId);

    std::vector<int> getAllSongsFromPlaylist(int playlistId);
    


private:
    SongDao _songDao;
    SongMetaDao _songMetaDao;
    AlbumDao _albumDao;
    LyricsDao _lyricsDao;
    PlaylistDao _playlistDao;
    PlaylistSongDao _playlistSongDao;

    int addAlbumIfNotExist(const FileService& fileService);

    int addSong(const FileService& fileService, int albumId);

    void addSongMetaData(const FileService& fileService, int songId, const std::string& filePath);

    void takeDataFromFile(const std::string& filePath);

    void fixAlbums();

    void fillDbWithSongsFromDir(const std::string& directory);

    void fillLyricsFromJsonsFromDir(const std::string& directory);
};

#endif // SONG_REPOSITORY_H
