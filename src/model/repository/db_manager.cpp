#include "db_manager.h"

DatabaseManager::DatabaseManager(const std::string& dbPath) : _db(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    _db.exec("PRAGMA foreign_keys = ON;");
    switch (checkVersion())
    {
    case 0:
        initDB();
        break;
    case 1:
        v1_to_v2();
        v2_to_v3();
        break;
    case 2:
        v2_to_v3();
        break;
    default:
        break;
    }
}

SQLite::Database& DatabaseManager::getDb() {
    return _db;
}

void DatabaseManager::createSongs(const std::string& tableName = "songs") {
    _db.exec("CREATE TABLE IF NOT EXISTS " + tableName + " ("
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
        "UNIQUE(album_id, track, disc), "
        "FOREIGN KEY (album_id) REFERENCES albums(id) ON DELETE SET NULL);"

        "CREATE INDEX IF NOT EXISTS idx_songs_album_id ON songs(album_id);"
    );
}

void DatabaseManager::createSongsMeta(const std::string& tableName = "songs_meta") {
    _db.exec("CREATE TABLE IF NOT EXISTS " + tableName + " ("
        "song_id INTEGER UNIQUE NOT NULL, "
        "duration INTEGER, "
        "bitrate INTEGER, "
        "channels INTEGER, "
        "sample_rate INTEGER, "
        "last_modified INTEGER NOT NULL, "
        "file TEXT UNIQUE, "
        "FOREIGN KEY (song_id) REFERENCES songs(id) ON DELETE CASCADE);"
    );
}

void DatabaseManager::createAlbums(const std::string& tableName = "albums") {
    _db.exec("CREATE TABLE IF NOT EXISTS " + tableName + " ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "title TEXT, "
        "artist TEXT, "
        "track_count INTEGER, "
        "disc_count INTEGER, "
        "compilation BOOLEAN DEFAULT FALSE, "
        "date TEXT, "
        "copyright TEXT, "
        "genre TEXT, "
        "cover_path TEXT, "
        "UNIQUE (title, artist));"
    );
}

void DatabaseManager::createPlaylists(const std::string& tableName = "playlists") {
    _db.exec("CREATE TABLE IF NOT EXISTS " + tableName + " ("
        "id INTEGER PRIMARY KEY, "
        "name TEXT, "
        "cover_path TEXT);"
    );
}

void DatabaseManager::createLyrics(const std::string& tableName = "lyrics") {
    _db.exec("CREATE TABLE IF NOT EXISTS " + tableName + " ("
        "song_id INTEGER NOT NULL, "
        "time_ms INTEGER NOT NULL, "
        "line TEXT NOT NULL, "
        "UNIQUE (song_id, time_ms), "
        "FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE);"

        "CREATE INDEX IF NOT EXISTS idx_lyrics_song_time ON lyrics(song_id, time_ms);"
    );
}

void DatabaseManager::createPlaylistSongs(const std::string& tableName = "playlist_songs") {
    _db.exec("CREATE TABLE IF NOT EXISTS " + tableName + " ("
        "playlist_id INTEGER NOT NULL, "
        "song_id INTEGER NOT NULL, "
        "PRIMARY KEY(playlist_id, song_id), "
        "FOREIGN KEY(playlist_id) REFERENCES playlists(id) ON DELETE CASCADE, "
        "FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE);"
    );
}

void DatabaseManager::initDB() {
    _db.exec("PRAGMA journal_mode = WAL;");

    createAlbums();
    createSongs();
    createSongsMeta();
    createPlaylists();
    createPlaylistSongs();
    createLyrics();
    
    _db.exec("CREATE TABLE IF NOT EXISTS metadata ("
        "key TEXT PRIMARY KEY, "
        "value TEXT);");
    
    _db.exec("INSERT OR IGNORE INTO metadata (key, value) VALUES ('db_version', '3');");
}

int DatabaseManager::checkVersion() {
    if (!_db.tableExists("metadata"))
        return 0;
    

    SQLite::Statement query(_db, "SELECT value FROM metadata WHERE key = 'db_version';");
    if (query.executeStep() && !query.isColumnNull(0)) {
        const char* value = query.getColumn(0).getText();

        return std::atoi(value);
    }

    return 0;
}

void DatabaseManager::v1_to_v2() {
    createAlbums("albums_temp");
    createPlaylists("playlists_temp");

    _db.exec("INSERT INTO albums_temp(id, title, artist, track_count, disc_count, compilation, date, copyright, genre, cover_path) " 
        "SELECT id, title, artist, track_count, disc_count, "
        "CASE WHEN compilation != 0 THEN 1 ELSE 0 END AS compilation, "
        "date, copyright, genre, NULL "
        "FROM albums");

    _db.exec("INSERT INTO playlists_temp(id, name, cover_path) " 
        "SELECT id, name, NULL "
        "FROM playlists");

    _db.exec("DROP TABLE albums; " 
        "ALTER TABLE albums_temp RENAME to albums;");

    _db.exec("DROP TABLE playlists; " 
        "ALTER TABLE playlists_temp RENAME to playlists;");

    _db.exec("UPDATE metadata SET value = '2' WHERE key = 'db_version';");

}

void DatabaseManager::v2_to_v3() {
    createSongs("songs_temp");
    createSongsMeta("songs_meta_temp");
    createLyrics("lyrics_temp");

    
    _db.exec("INSERT INTO songs_temp " 
        "SELECT * FROM songs");
    _db.exec("INSERT INTO songs_meta_temp " 
        "SELECT * FROM songs_meta");
    _db.exec("INSERT INTO lyrics_temp " 
        "SELECT * FROM lyrics");

        
    _db.exec("DROP TABLE songs; " 
        "ALTER TABLE songs_temp RENAME to songs;");
    _db.exec("DROP TABLE songs_meta; " 
        "ALTER TABLE songs_meta_temp RENAME to songs_meta;");
    _db.exec("DROP TABLE lyrics; " 
        "ALTER TABLE lyrics_temp RENAME to lyrics;");

        
    _db.exec("UPDATE metadata SET value = '3' WHERE key = 'db_version';");
}