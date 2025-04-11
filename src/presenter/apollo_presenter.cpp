#include "apollo_presenter.h"

ApolloPresenter::ApolloPresenter(ISongRepository& sr, IAlbumRepository& ar, IPlaylistRepository& pr,
                                 IFileService& fs) : _sr(sr), _ar(ar), _pr(pr), _fs(fs) {}
ApolloPresenter::~ApolloPresenter() = default;

const std::vector<Song> ApolloPresenter::getAllSongs() const {
    std::vector<SongEntity> seList = _sr.getAllSongs();

    std::vector<Song> songs;
    songs.reserve(seList.size());
    for (auto &&se : seList)
        songs.push_back(toSong(se, _sr.getSongMetaById(se.id)));

    return songs;
}
const Song ApolloPresenter::getSong(int id) const {
    SongEntity song = _sr.getSongById(id);
    SongMetaEntity songMeta = _sr.getSongMetaById(id);
    
    return toSong(song, songMeta);
}
const Lyrics ApolloPresenter::getSongLyrics(int songId) const {
    std::vector<LyricLineEntity> lyrics = _sr.getSongLyricsById(songId);

    Lyrics lrc;
    lrc.songId = songId;
    lrc.lrc.reserve(lyrics.size());

    for (auto &&line : lyrics)
        lrc.lrc.emplace_back(line.time_ms, line.line);
    
    return lrc;
}



const std::vector<Album> ApolloPresenter::getAllAlbums() const {
    std::vector<AlbumEntity> albumsEntities = _ar.getAllAlbums();

    std::vector<Album> albums;
    albums.reserve(albumsEntities.size());
    for (auto &&albumEntity : albumsEntities)
        albums.push_back(toAlbum(albumEntity));
    
    return albums;
}
const Album ApolloPresenter::getAlbum(int id) const {
    return toAlbum(_ar.getAlbumById(id));
}



const std::vector<Playlist> ApolloPresenter::getAllPlaylists() const {
    std::vector<PlaylistEntity> peList = _pr.getAllPlaylists();

    std::vector<Playlist> playlists;
    playlists.reserve(peList.size());
    for (auto &&pe : peList)
        playlists.push_back(toPlaylist(pe, _pr.getPlaylistSongs(pe.id)));

    return playlists;
}
const Playlist ApolloPresenter::getPlaylist(int id) const {
    PlaylistEntity pe = _pr.getPlaylistById(id);
    
    return toPlaylist(pe, _pr.getPlaylistSongs(id));
}
int ApolloPresenter::createPlaylist(const std::string name) {
    return _pr.createPlaylist(name);
}
bool ApolloPresenter::renamePlaylist(int playlistId, const std::string newName) {
    return _pr.renamePlaylist(playlistId, newName);
}
bool ApolloPresenter::deletePlaylist(int playlistId) {
    return _pr.deletePlaylist(playlistId);
}
bool ApolloPresenter::addSongToPlaylist(int playlistId, int songId){
    return _pr.addSongToPlaylist(playlistId, songId);
}
bool ApolloPresenter::removeSongFromPlaylist(int playlistId, int songId){
    return _pr.removeSongFromPlaylist(playlistId, songId);
}

bool ApolloPresenter::uploadPlaylistCover(int id, const char* bytes, std::streamsize size) {
    std::string name = std::to_string(id) + '_' + _pr.getPlaylistById(id).name;
    std::string path = _fs.saveBytesToFile(name, bytes, size);
    if (path.empty())
        return false;
    
    return _pr.updatePlaylistCoverPath(id, path);
}




Song ApolloPresenter::toSong(const SongEntity &se, const SongMetaEntity &sme) {
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
Album ApolloPresenter::toAlbum(const AlbumEntity &ae) {
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
Playlist ApolloPresenter::toPlaylist(const PlaylistEntity &pe, const std::vector<int>&& songs) {   
    return {pe.id, pe.name, songs};
}
