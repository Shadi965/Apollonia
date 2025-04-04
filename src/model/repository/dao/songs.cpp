#include "songs.h"

// CREATE TABLE IF NOT EXISTS songs (
//     id INTEGER PRIMARY KEY, 
//     title TEXT, 
//     artist TEXT, 
//     composer TEXT, 
//     album_id INTEGER, 
//     track INTEGER, 
//     disc INTEGER, 
//     date TEXT, 
//     copyright TEXT, 
//     genre TEXT, 
//     FOREIGN KEY (album_id) REFERENCES albums(id) ON DELETE SET NULL);

std::vector<Song> SongDao::getAllSongs() {
    std::vector<Song> songs;
    SQLite::Statement query(_db, "SELECT * FROM songs");

    while (query.executeStep()) {
        songs.push_back({
            query.getColumn(0).getInt(),
            query.getColumn(1).getString(),
            query.getColumn(2).getString(),
            query.getColumn(3).getString(),
            query.getColumn(4).getInt(),
            query.getColumn(5).getInt(),
            query.getColumn(6).getInt(),
            query.getColumn(7).getString(),
            query.getColumn(8).getString(),
            query.getColumn(9).getString()
        });
    }

    return songs;
}


Song SongDao::getSongById(int songId) {
    SQLite::Statement query(_db, 
        "SELECT * FROM songs WHERE id = ?"
    );
    query.bind(1, songId);
    
    // TODO: Реализовать нормальные исключения
    if (!query.executeStep()) 
        throw std::runtime_error("No song found with id: " + std::to_string(songId));

    return {
        query.getColumn(0).getInt(),
        query.getColumn(1).getString(),
        query.getColumn(2).getString(),
        query.getColumn(3).getString(),
        query.getColumn(4).getInt(),
        query.getColumn(5).getInt(),
        query.getColumn(6).getInt(),
        query.getColumn(7).getString(),
        query.getColumn(8).getString(),
        query.getColumn(9).getString()
    };
}


int64_t SongDao::insertSong(const Song& song) {
    SQLite::Statement query(_db, 
        "INSERT INTO songs (title, artist, composer, album_id, track, disc, date, copyright, genre) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.bind(1, song.title);
    query.bind(2, song.artist);
    query.bind(3, song.composer);
    query.bind(4, song.albumId);
    query.bind(5, song.track);
    query.bind(6, song.disc);
    query.bind(7, song.date);
    query.bind(8, song.copyright);
    query.bind(9, song.genre);

    query.executeStep();
    
    return _db.getLastInsertRowid();
}

void SongDao::deleteSongById(int songId) {
    SQLite::Statement query(_db, "DELETE FROM songs WHERE id = ?");
    query.bind(1, songId);
    query.executeStep();
}

std::string SongDao::getNewestSongDateInAlbum(int64_t albumId) {
    SQLite::Statement query(_db, "SELECT MAX(date) FROM songs WHERE album_id = ?");
    query.bind(1, albumId);

    if (query.executeStep())
        return query.getColumn(0).getString();
        
    return "";
}

std::string SongDao::getMostFrequentGenreInAlbum(int64_t albumId) {
    SQLite::Statement query(_db, "SELECT genre, COUNT(genre) as count "
                                "FROM songs WHERE album_id = ? "
                                "GROUP BY genre ORDER BY count DESC LIMIT 1");
    query.bind(1, albumId);

    if (query.executeStep())
        return query.getColumn(0).getString();

    return "";
}

std::vector<std::tuple<int, std::string, std::string, int>> SongDao::getTitleArtistAlbumTuples() {
    std::vector<std::tuple<int, std::string, std::string, int>> tuples;

    SQLite::Statement query(_db, "SELECT id, title, artist, album_id FROM songs");

    while (query.executeStep())
        tuples.push_back({
            query.getColumn(0).getInt(),
            query.getColumn(1).getString(),
            query.getColumn(2).getString(),
            query.getColumn(3).getInt()
        });

    return tuples;
}
