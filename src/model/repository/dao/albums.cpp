#include "albums.h"

// CREATE TABLE IF NOT EXISTS albums (
//     id INTEGER PRIMARY KEY AUTOINCREMENT, 
//     title TEXT, 
//     artist TEXT, 
//     track_count INTEGER, 
//     disc_count INTEGER, 
//     compilation INTEGER, 
//     date TEXT, 
//     copyright TEXT, 
//     genre TEXT, 
//     UNIQUE (title, artist));


std::vector<Album> AlbumDao::getAllAlbums() {
        std::vector<Album> albums;
        SQLite::Statement query(_db, "SELECT * FROM albums");

        while (query.executeStep()) {
            albums.push_back({
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getInt(),
                query.getColumn(4).getInt(),
                query.getColumn(5).getInt() != 0,
                query.getColumn(6).getString(),
                query.getColumn(7).getString(),
                query.getColumn(8).getString()
            });
        }

        return albums;
    }
    
Album AlbumDao::getAlbumById(int albumId) {
        SQLite::Statement query(_db, "SELECT * FROM albums WHERE id = ?");
        query.bind(1, albumId);

        // TODO: Реализовать нормальные исключения
        if (!query.executeStep()) 
            throw std::runtime_error("No album found with id: " + std::to_string(albumId));

        return {
            query.getColumn(0).getInt(),
            query.getColumn(1).getString(),
            query.getColumn(2).getString(),
            query.getColumn(3).getInt(),
            query.getColumn(4).getInt(),
            query.getColumn(5).getInt() != 0,
            query.getColumn(6).getString(),
            query.getColumn(7).getString(),
            query.getColumn(8).getString()
        };
    }

std::string AlbumDao::getAlbumTitleById(int albumId) {
        SQLite::Statement query(_db, "SELECT title FROM albums WHERE id = ?");
        query.bind(1, albumId);

        // TODO: Реализовать нормальные исключения
        if (!query.executeStep()) 
            throw std::runtime_error("No album found with id: " + std::to_string(albumId));

        return query.getColumn(0).getString();
    }

int64_t AlbumDao::isAlbumExists(const std::string& artist, const std::string& title) {
        SQLite::Statement query(_db, "SELECT id FROM albums WHERE title = ? AND artist = ?");
        query.bind(1, title);
        query.bind(2, artist);
        
        return query.executeStep() ? query.getColumn(0).getInt64() : 0LL;
    }

int64_t AlbumDao::insertAlbum(const Album& album) {
        // TODO: Возможна ошибка из-за дублирования title + artist
        SQLite::Statement query(_db, 
            "INSERT INTO albums (title, artist, track_count, disc_count, compilation, date, copyright, genre) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"
        );
        query.bind(1, album.title);
        query.bind(2, album.artist);
        query.bind(3, album.track_count);
        query.bind(4, album.disc_count);
        query.bind(5, album.compilation);
        query.bind(6, album.date);
        query.bind(7, album.copyright);
        query.bind(8, album.genre);

        query.executeStep();

        return _db.getLastInsertRowid();
    }
    
std::vector<int> AlbumDao::getAllAlbumsIds() {
        std::vector<int> ids;

        SQLite::Statement query(_db, "SELECT id FROM albums");

        while (query.executeStep())
            ids.push_back(query.getColumn(0).getInt());

        return ids;
    }

int AlbumDao::updateAlbumDateAndGenre(int albumId, const std::string& newDate, const std::string& newGenre) {
        SQLite::Statement query(_db, "UPDATE albums SET date = ?, genre = ? WHERE id = ?");
        query.bind(1, newDate);
        query.bind(2, newGenre);
        query.bind(3, albumId);

        query.executeStep();
        return _db.getChanges();
    }
