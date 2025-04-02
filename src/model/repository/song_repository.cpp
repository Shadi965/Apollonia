#include <vector>
#include <filesystem>

#include "song_repository.h"

Song SongRepository::getSongById(int songId) {
    return _songDao.getSongById(songId);
}

std::vector<Song> SongRepository::getSongs() {
    return _songDao.getAllSongs();
}

SongMeta SongRepository::getSongMetaInfo(int songId) {
    return _songMetaDao.getSongDataById(songId);
}

std::string SongRepository::getSongPath(int songId) {
    return _songMetaDao.getFilePathBySongId(songId);
}

Album SongRepository::getAlbumById(int albumId) {
    return _albumDao.getAlbumById(albumId);
}

std::vector<Album> SongRepository::getAlbums() {
    return _albumDao.getAllAlbums();
}

void SongRepository::initDB(const std::string& musicDir, const std::string& lyricsJsonDir) {
    fillDbWithSongsFromDir(musicDir);
    fillLyricsFromJsonsFromDir(lyricsJsonDir);
}




int SongRepository::addAlbumIfNotExist(const FileService& fileService) {
    Album album;

    album.title = fileService.getSongAlbumName();
    album.artist = fileService.getSongAlbumArtist();
    album.id = _albumDao.isAlbumExists(album.artist, album.title);

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

int SongRepository::addSong(const FileService& fileService, int albumId) {
    Song song;
    
    song.albumId = albumId;

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

void SongRepository::addSongMetaData(const FileService& fileService, int songId, const std::string& filePath) {
    SongMeta songMeta;

    songMeta.songId = songId;

    songMeta.duration = fileService.getSongDuration();
    songMeta.bitrate = fileService.getSongBitrate();
    songMeta.channels = fileService.getSongChannelsCount();
    songMeta.sampleRate = fileService.getSongSampleRate();
    songMeta.lastModified = fileService.last_write_time(filePath);

    songMeta.file = filePath;

    _songMetaDao.insertSongData(songMeta);
}

void SongRepository::takeDataFromFile(const std::string& filePath) {
    FileService fileService(filePath);
    if (!fileService.isOpen() || !fileService.isMedia()) 
        return;
    
    int albumId = addAlbumIfNotExist(fileService);
    int songId = addSong(fileService, albumId);
    addSongMetaData(fileService, songId, filePath);
}

void SongRepository::fixAlbums() {
    for (int albumId : _albumDao.getAllAlbumsIds()) {
        std::string date = _songDao.getOldestSongDateInAlbum(albumId);
        std::string genre = _songDao.getMostFrequentGenreInAlbum(albumId);

        _albumDao.updateAlbumDateAndGenre(albumId, date, genre);
    }
}

void SongRepository::fillDbWithSongsFromDir(const std::string& directory) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            takeDataFromFile(entry.path().string());
        }
    }
    fixAlbums();
}

void SongRepository::fillLyricsFromJsonsFromDir(const std::string& directory) {
    FileService fileService;
    fileService.scanDirWithLyricJsons(directory);

    for (auto tuple : _songDao.getTitleArtistAlbumTuples()) {
        auto [id, title, artist, albumId] = tuple;
        std::string albumName = _albumDao.getAlbumTitleById(albumId);
        auto lyrics = fileService.getSongLyrics(artist, albumName, title);
        for (auto line : lyrics)
            _lyricsDao.addLyricLine(id, line.first, line.second);
        
    }
    
}
