#include <algorithm>

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
const FileData ApolloPresenter::getFileChunk(int id, size_t start, size_t end) const {
    if (end == 0) end = start + 524'288;
    std::string path = _sr.getSongPathById(id);
    return _fs.getChunk(path, start, end);
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


static void sanitizeFilename(std::string& name) {
    for (char& ch : name) {
        if (ch == '/' || ch == '\\' || ch == '*' || ch == '?' || ch == '"' ||
            ch == '<' || ch == '>' || ch == '|' || ch == ':' || ch == ' ' ||
            std::iscntrl(static_cast<unsigned char>(ch))) {
            ch = '_';
        }
    }
}
const FileData ApolloPresenter::dloadAlbumCover(int id) const {
    std::filesystem::path path = _ar.getAlbumCoverPath(id);
    return _fs.getFile(path);
}
bool ApolloPresenter::uploadAlbumCover(int id, const char* bytes, std::streamsize size, std::string fileExtension) {
    std::string name = std::to_string(id) + '_' + _ar.getAlbumById(id).title + fileExtension;
    sanitizeFilename(name);
    std::string path = _fs.saveAlbumCover(name, bytes, size);
    if (path.empty())
        return false;
    
    return _ar.updateAlbumCoverPath(id, path);
}

const std::vector<int> ApolloPresenter::getAlbumSongs(int albumId) const {
    return _ar.getAlbumSongs(albumId);
}


const std::vector<Playlist> ApolloPresenter::getAllPlaylists() const {
    std::vector<PlaylistEntity> peList = _pr.getAllPlaylists();

    std::vector<Playlist> playlists;
    playlists.reserve(peList.size());
    for (auto &&pe : peList)
        playlists.push_back(toPlaylist(pe));

    return playlists;
}
const Playlist ApolloPresenter::getPlaylist(int id) const {
    PlaylistEntity pe = _pr.getPlaylistById(id);
    
    return toPlaylist(pe);
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
bool ApolloPresenter::addSongToPlaylist(int playlistId, int songId, double position){
    return _pr.addSongToPlaylist(playlistId, songId, position);
}
bool ApolloPresenter::removeSongFromPlaylist(int playlistId, int songId){
    return _pr.removeSongFromPlaylist(playlistId, songId);
}
bool ApolloPresenter::updateSongPosition(int playlistId, int songId, double position) {
    return _pr.updateSongPosition(playlistId, songId, position);
}

std::vector<int> ApolloPresenter::addSongsToPlaylist(int playlistId, std::vector<std::pair<int, double>>& positionedSongs) {
    std::vector<PlaylistSongEntity> psList;
    for (auto &&[songId, position] : positionedSongs)
        psList.push_back({playlistId, songId, position});
    
    std::vector<PlaylistSongEntity> notAdded = _pr.addSongsToPlaylist(psList);
    std::vector<int> notAddedIds;
    for (auto &&ps : notAdded)
        notAddedIds.push_back(ps.song_id);
    
    return notAddedIds;
}

const FileData ApolloPresenter::dloadPlaylistCover(int id) const {
    std::filesystem::path path = _pr.getPlaylistCoverPath(id);
    return _fs.getFile(path);
}
bool ApolloPresenter::uploadPlaylistCover(int id, const char* bytes, std::streamsize size, std::string fileExtension) {
    std::string name = std::to_string(id) + '_' + _pr.getPlaylistById(id).name + fileExtension;
    std::replace(name.begin(), name.end(), ' ', '_');
    std::string path = _fs.savePlaylistCover(name, bytes, size);
    if (path.empty())
        return false;
    
    return _pr.updatePlaylistCoverPath(id, path);
}

const std::vector<std::pair<int, double>> ApolloPresenter::getPlaylistSongs(int playlistId) const {
    return _pr.getPlaylistSongs(playlistId);
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
Playlist ApolloPresenter::toPlaylist(const PlaylistEntity &pe) {   
    return {pe.id, pe.name};
}
