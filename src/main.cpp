#define CROW_MAIN
#include <crow.h>
#include "database.h"
#include "api.h"
#include <iostream>

#define PORT 8080
#define DB_PATH "./songs.db"
#define JSON_PATH "./tracks_info.json"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <music_folder>" << std::endl;
        return 1;
    }
    
    std::string musicFolder = argv[1];
    crow::SimpleApp app;
    Database db(DB_PATH, musicFolder, JSON_PATH);

    setupRoutes(app, db);
    app.port(PORT).multithreaded().run();

    return 0;
}