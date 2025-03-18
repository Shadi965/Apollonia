#ifndef DATABASE_H
#define DATABASE_H

#include "SQLiteCpp/SQLiteCpp.h"
#include <vector>
#include <string>

struct Song {
    int id;
    std::string title;
    std::string artist;
    std::string file;
    int duration;
};

class Database {
public:
    Database(const std::string& dbPath);
    std::vector<Song> getAllSongs();
    Song getSongById(int id);

    void updateDatabase(const std::string& musicFolder);

private:
    SQLite::Database db;
    
};

#endif // DATABASE_H
