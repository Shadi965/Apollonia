#include "presenter.h"

Presenter::Presenter(ISongRepository& sr, IAlbumRepository& ar, IPlaylistRepository& pr) : _sr(sr), _ar(ar), _pr(pr) {}
Presenter::~Presenter() = default;

const std::vector<Song> Presenter::getAllSongs() const {
    std::vector<SongEntity> seList = _sr.getAllSongs();

    std::vector<Song> songs(seList.size());
    for (auto &&se : seList)
        songs.push_back(toSong(se, _sr.getSongMetaById(se.id)));

    return songs;
}
const Song Presenter::getSong(int id) const {
    SongEntity song = _sr.getSongById(id);
    SongMetaEntity songMeta = _sr.getSongMetaById(id);
    
    return toSong(song, songMeta);
}
const Lyrics Presenter::getSongLyrics(int songId) const {
    std::vector<LyricLineEntity> lyrics = _sr.getSongLyricsById(songId);

    Lyrics lrc;
    lrc.songId = lyrics[0].song_id;
    lrc.lrc.reserve(lyrics.size());

    for (auto &&line : lyrics)
        lrc.lrc.emplace_back(line.time_ms, line.line);
    
    return lrc;
}



const std::vector<Album> Presenter::getAllAlbums() const {
    std::vector<AlbumEntity> albumsEntities = _ar.getAllAlbums();

    std::vector<Album> albums(albumsEntities.size());
    for (auto &&albumEntity : albumsEntities)
        albums.push_back(toAlbum(albumEntity));
    
    return albums;
}
const Album Presenter::getAlbum(int id) const {
    return toAlbum(_ar.getAlbumById(id));
}



const std::vector<Playlist> Presenter::getAllPlaylists() const {
    std::vector<PlaylistEntity> peList = _pr.getAllPlaylists();

    std::vector<Playlist> playlists(peList.size());
    for (auto &&pe : peList)
        playlists.push_back(toPlaylist(pe, _pr.getPlaylistSongs(pe.id)));

    return playlists;
}
const Playlist Presenter::getPlaylist(int id) const {
    PlaylistEntity pe = _pr.getPlaylistById(id);
    
    return toPlaylist(pe, _pr.getPlaylistSongs(id));
}
int Presenter::createPlaylist(const std::string name) {
    return _pr.createPlaylist(name);
}
bool Presenter::renamePlaylist(int playlistId, const std::string newName) {
    return _pr.renamePlaylist(playlistId, newName);
}
bool Presenter::deletePlaylist(int playlistId) {
    return _pr.deletePlaylist(playlistId);
}
bool Presenter::addSongToPlaylist(int playlistId, int songId){
    return _pr.addSongToPlaylist(playlistId, songId);
}
bool Presenter::removeSongFromPlaylist(int playlistId, int songId){
    return _pr.removeSongFromPlaylist(playlistId, songId);
}




Song Presenter::toSong(const SongEntity &se, const SongMetaEntity &sme) {
    return {
        se.id,
        se.title,
        se.artist,
        se.composer,
        se.album_id,
        se.track,
        se.disc,
        se.date,
        se.copyright,
        se.genre,
        sme.duration,
        sme.bitrate,
        sme.channels,
        sme.sample_rate
    };
}
Album Presenter::toAlbum(const AlbumEntity &ae) {
    return {
        ae.id,
        ae.title,
        ae.artist,
        ae.track_count,
        ae.disc_count,
        ae.compilation,
        ae.date,
        ae.copyright,
        ae.genre
    };
}
Playlist Presenter::toPlaylist(const PlaylistEntity &pe, const std::vector<int>&& songs) {   
    return {pe.id, pe.name, songs};
}
