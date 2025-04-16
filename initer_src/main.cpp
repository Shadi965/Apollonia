#include <iostream>
#include <string>

#include "db_manager.h"
#include "init_db.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <database_name> <music_folder> <lyrics_json_folder>\n";
        return 1;
    }

    std::string dbName = argv[1];
    std::string musicFolder = argv[2];
    std::string lyricsJsonFolder = argv[3];

    try {
        DatabaseManager db_manager(dbName);
        InitDB db_init(db_manager.getDb());

        db_init.initDB(musicFolder, lyricsJsonFolder);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
