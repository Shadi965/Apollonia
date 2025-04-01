#include "db_manager.h"


void DatabaseManager::createSongInfo() {
    db.exec("CREATE TABLE IF NOT EXISTS songs ("
        "id INTEGER PRIMARY KEY, "
        "title TEXT, "
        "artist TEXT, "
        "composer TEXT, "
        "album_id INTEGER, "
        "track INTEGER, "
        "disc INTEGER, "
        "date TEXT, "
        "copyright TEXT, "
        "genre TEXT, "
        "FOREIGN KEY (album_id) REFERENCES albums(id) ON DELETE SET NULL);"

        "CREATE INDEX IF NOT EXISTS idx_songs_album_id ON songs(album_id);"
    );
}

void DatabaseManager::createSongMeta() {
    db.exec("CREATE TABLE IF NOT EXISTS songs_meta ("
        "song_id INTEGER, "
        "duration INTEGER, "
        "bitrate INTEGER, "
        "channels INTEGER, "
        "sample_rate INTEGER, "
        "last_modified INTEGER NOT NULL, "
        "file TEXT UNIQUE, "
        "FOREIGN KEY (song_id) REFERENCES songs(id) ON DELETE CASCADE);"
    );
}

void DatabaseManager::createAlbums() {
    db.exec("CREATE TABLE IF NOT EXISTS albums ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "title TEXT, "
        "artist TEXT, "
        "track_count INTEGER, "
        "disc_count INTEGER, "
        "compilation INTEGER, "
        "date TEXT, "
        "copyright TEXT, "
        "genre TEXT, "
        "UNIQUE (title, artist));"
    );
}

void DatabaseManager::createPlaylists() {
    db.exec("CREATE TABLE IF NOT EXISTS playlists ("
        "id INTEGER PRIMARY KEY, "
        "name TEXT);");
}

void DatabaseManager::createLyrics() {
    db.exec("CREATE TABLE IF NOT EXISTS lyrics ("
        "song_id INTEGER NOT NULL, "
        "time_ms INTEGER NOT NULL, "
        "line TEXT NOT NULL, "
        "FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE);");

    db.exec("CREATE INDEX IF NOT EXISTS idx_lyrics_song_time ON lyrics(song_id, time_ms);");
}

void DatabaseManager::createPlaylistSongs() {
    db.exec("CREATE TABLE IF NOT EXISTS playlist_songs ("
        "playlist_id INTEGER NOT NULL, "
        "song_id INTEGER NOT NULL, "
        "PRIMARY KEY(playlist_id, song_id), "
        "FOREIGN KEY(playlist_id) REFERENCES playlists(id) ON DELETE CASCADE, "
        "FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE);");
}

void DatabaseManager::initDB() {
    db.exec("PRAGMA foreign_keys = ON;");

    createAlbums();
    createSongInfo();
    createSongMeta();
    createPlaylists();
    createPlaylistSongs();
    createLyrics();
    
    db.exec("CREATE TABLE IF NOT EXISTS metadata ("
        "key TEXT PRIMARY KEY, "
        "value TEXT);");
    
    db.exec("INSERT OR IGNORE INTO metadata (key, value) VALUES ('db_version', '1');");
}
