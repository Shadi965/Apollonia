#include "fs.h"

#include "albums.h"
#include "lyrics.h"
#include "songs_meta.h"
#include "songs.h"

class InitDB {
    AlbumDao _albumDao;
    SongDao _songDao;
    LyricsDao _lyricsDao;
    SongMetaDao _songMetaDao;

public:

    InitDB(SQLite::Database& db) : _albumDao(db), _songDao(db), _lyricsDao(db), _songMetaDao(db) {}

    void initDB(const std::string& musicDir, const std::string& lyricsJsonDir) {
        fillDbWithSongsFromDir(musicDir);
        fillLyricsFromJsonsFromDir(lyricsJsonDir);
    }
    
    void fixAlbums() {
        for (int albumId : _albumDao.getAllAlbumsIds()) {
            std::string date = _songDao.getNewestSongDateInAlbum(albumId);
            std::string genre = _songDao.getMostFrequentGenreInAlbum(albumId);
            int trackCount = _songDao.getSongsFromAlbum(albumId).size();
    
            _albumDao.updateAlbumDateAndGenre(albumId, date, genre);
            _albumDao.updateAlbumTrackCount(albumId, trackCount);
        }
    }
    
    void fillDbWithSongsFromDir(const std::string& directory) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                takeDataFromFile(entry.path().string());
            }
        }
        fixAlbums();
    }
    
    void fillLyricsFromJsonsFromDir(const std::string& directory) {
        FileService fileService;
        fileService.scanDirWithLyricJsons(directory);
    
        for (auto tuple : _songDao.getTitleArtistAlbumTuples()) {
            std::string albumName = _albumDao.getAlbumTitleById(tuple.album_id);
            auto lyrics = fileService.getSongLyrics(tuple.artist, albumName, tuple.title);
            for (auto line : lyrics)
                _lyricsDao.addLyricLine(tuple.id, line.first, line.second);
    
        }
    
    }
    
    void takeDataFromFile(const std::string& filePath) {
        FileService fileService(filePath);
        if (!fileService.isOpen() || !fileService.isMedia()) 
            return;
    
        int albumId = addAlbumIfNotExist(fileService);
        int songId = addSong(fileService, albumId);
        addSongMetaData(fileService, songId, filePath);
    }

    int addAlbumIfNotExist(const FileService& fileService) {
        AlbumEntity album;
    
        album.title = fileService.getSongAlbumName();
        album.artist = fileService.getSongAlbumArtist();
        album.id = _albumDao.checkAlbumExists(album.artist, album.title);
    
        if (!album.id) {
            album.track_count = fileService.getSongAlbumTrackCount();
            album.disc_count = fileService.getSongAlbumDiscCount();
            album.compilation= fileService.getIsSongAlbumCompilation();
            album.date = fileService.getSongDate();
            album.copyright = fileService.getSongCopyright();
            album.genre = fileService.getSongGenre();
    
            album.id = _albumDao.insertAlbum(album);
        }
    
        return album.id;
    }
    
    int addSong(const FileService& fileService, int albumId) {
        SongEntity song;
    
        song.album_id = albumId;
    
        song.title = fileService.getSongTitle();
        song.artist = fileService.getSongArtist();
        song.composer = fileService.getSongComposer();
        song.track = fileService.getSongTrackNum();
        song.disc = fileService.getSongDiscNum();
        song.date = fileService.getSongDate();
        song.copyright = fileService.getSongCopyright();
        song.genre = fileService.getSongGenre();
    
        song.id = _songDao.insertSong(song);
    
        return song.id;
    }
    
    void addSongMetaData(const FileService& fileService, int songId, const std::string& filePath) {
        SongMetaEntity songMeta;
    
        songMeta.song_id = songId;
    
        songMeta.duration = fileService.getSongDuration();
        songMeta.bitrate = fileService.getSongBitrate();
        songMeta.channels = fileService.getSongChannelsCount();
        songMeta.sample_rate = fileService.getSongSampleRate();
        songMeta.last_modified = fileService.last_write_time(filePath);
    
        songMeta.file = filePath;
    
        _songMetaDao.insertSongData(songMeta);
    }
};