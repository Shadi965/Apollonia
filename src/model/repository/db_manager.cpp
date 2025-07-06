#include "db_manager.h"
#include "dao/playlist_songs.h"
DatabaseManager::DatabaseManager(const std::string& dbPath) : _db(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    _db.exec("PRAGMA foreign_keys = ON;");
    if (getDbVersion() == 0)
        initDB();
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

void DatabaseManager::createSongsFts(const std::string& tableName = "songs") {
        _db.exec("CREATE VIRTUAL TABLE IF NOT EXISTS " + tableName + "_fts USING fts5("
        "title, "
        "artist, "
        "content='" + tableName + "', "
        "content_rowid='id');"
    );

    _db.exec("CREATE TRIGGER IF NOT EXISTS " + tableName + "_ai AFTER INSERT ON " + tableName + " BEGIN "
        "INSERT INTO " + tableName + "_fts(rowid, title, artist) "
        "VALUES (new.id, new.title, new.artist);"
        "END;"
    );

    _db.exec("CREATE TRIGGER IF NOT EXISTS " + tableName + "_ad AFTER DELETE ON " + tableName + " BEGIN "
        "INSERT INTO " + tableName + "_fts(" + tableName + "_fts, rowid, title, artist) "
        "VALUES('delete', old.id, old.title, old.artist);"
        "END;"
    );

    _db.exec("CREATE TRIGGER IF NOT EXISTS " + tableName + "_au AFTER UPDATE ON " + tableName + " BEGIN "
        "INSERT INTO " + tableName + "_fts(" + tableName + "_fts, rowid, title, artist) "
        "VALUES('delete', old.id, old.title, old.artist);"
        "INSERT INTO " + tableName + "_fts(rowid, title, artist) "
        "VALUES (new.id, new.title, new.artist);"
        "END;"
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

void DatabaseManager::createLyricsFts(const std::string& tableName = "lyrics") {
    _db.exec("CREATE VIRTUAL TABLE IF NOT EXISTS " + tableName + "_fts USING fts5("
        "song_id UNINDEXED, "
        "line, "
        "content='" + tableName + "', "
        "content_rowid='rowid');"
    );

    _db.exec("CREATE TRIGGER IF NOT EXISTS " + tableName + "_ai AFTER INSERT ON " + tableName + " BEGIN "
        "INSERT INTO " + tableName + "_fts(rowid, song_id, line) "
        "VALUES (new.rowid, new.song_id, new.line);"
        "END;"
    );

    _db.exec("CREATE TRIGGER IF NOT EXISTS " + tableName + "_ad AFTER DELETE ON " + tableName + " BEGIN "
        "INSERT INTO " + tableName + "_fts(" + tableName + "_fts, rowid, song_id, line) "
        "VALUES('delete', old.rowid, old.song_id, old.line);"
        "END;"
    );

    _db.exec("CREATE TRIGGER IF NOT EXISTS " + tableName + "_au AFTER UPDATE ON " + tableName + " BEGIN "
        "INSERT INTO " + tableName + "_fts(" + tableName + "_fts, rowid, song_id, line) "
        "VALUES('delete', old.rowid, old.song_id, old.line);"
        "INSERT INTO " + tableName + "_fts(rowid, song_id, line) "
        "VALUES (new.rowid, new.song_id, new.line);"
        "END;"
    );
}

void DatabaseManager::rebuildFts() {
    _db.exec("INSERT INTO songs_fts(songs_fts) VALUES('rebuild');");
    _db.exec("INSERT INTO lyrics_fts(lyrics_fts) VALUES('rebuild');");
}

void DatabaseManager::createPlaylistSongs(const std::string& tableName = "playlist_songs") {
    _db.exec("CREATE TABLE IF NOT EXISTS " + tableName + " ("
        "playlist_id INTEGER NOT NULL, "
        "song_id INTEGER NOT NULL, "
        "position REAL, "
        "PRIMARY KEY(playlist_id, song_id), "
        "UNIQUE(playlist_id, position), "
        "FOREIGN KEY(playlist_id) REFERENCES playlists(id) ON DELETE CASCADE, "
        "FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE);"
    );
}

void DatabaseManager::initDB() {
    _db.exec("PRAGMA journal_mode = WAL;");

    createAlbums();
    createSongs();
    createSongsFts();
    createSongsMeta();
    createPlaylists();
    createPlaylistSongs();
    createLyrics();
    createLyricsFts();
    _db.exec("CREATE TABLE IF NOT EXISTS metadata ("
        "key TEXT PRIMARY KEY, "
        "value TEXT);");
    
    _db.exec("INSERT OR IGNORE INTO metadata (key, value) VALUES ('db_version', '5');");
}

int DatabaseManager::getDbVersion() {
    if (!_db.tableExists("metadata"))
        return 0;
    

    SQLite::Statement query(_db, "SELECT value FROM metadata WHERE key = 'db_version';");
    if (query.executeStep() && !query.isColumnNull(0)) {
        const char* value = query.getColumn(0).getText();

        return std::atoi(value);
    }

    return 0;
}