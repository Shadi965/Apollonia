#define CROW_MAIN

#include <getopt.h>
#include <iostream>
#include <string>

#include "routes.h"
#include "apollo_presenter.h"
#include "apollo_repository.h"
#include "file_service.h"
#include "db_manager.h"

#define PORT 8080
#define DB_PATH "/home/shadi/Apollo/apollo.db"
#define COVER_PATH "/home/shadi/Apollo/covers/"

static void parseArguments(int argc, char* argv[], int& port, std::string& dbPath, std::string& coversFolder);

int main(int argc, char* argv[]) {
    int port = PORT;
    std::string dbPath = DB_PATH;
    std::string coversFolder = COVER_PATH;

    parseArguments(argc, argv, port, dbPath, coversFolder);

    DatabaseManager dbManager(dbPath);
    ApolloRepository repository(dbManager.getDb());
    FileService fileService(coversFolder);

    ApolloPresenter presenter(repository, repository, repository, fileService);

    RoutesManager routesManager(port);

    routesManager.regSongRoutes(presenter);
    routesManager.regAlbumRoutes(presenter);
    routesManager.regPlaylistRoutes(presenter);

    routesManager.runApp();

    return 0;
}

static void parseArguments(int argc, char* argv[], int& port, std::string& dbPath, std::string& coversFolder) {
    const char* const short_opts = "p:d:c:";
    const option long_opts[] = {
        {"port",   required_argument, nullptr, 'p'},
        {"db",     required_argument, nullptr, 'd'},
        {"covers", required_argument, nullptr, 'c'},
        {nullptr,  0,                 nullptr,  0}
    };

    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
        if (opt == -1)
            break;

        switch (opt) {
            case 'p':
                port = std::atoi(optarg);
                break;
            case 'd':
                dbPath = optarg;
                break;
            case 'c':
                coversFolder = optarg;
                break;
            case '?':
            default:
                std::cerr << "Usage: " << argv[0]
                          << " [-p port] [-d database_path] [-c covers_folder]\n"
                          << "  --port or -p      Set server port (default " << PORT << ")\n"
                          << "  --db or -d        Path to database file (default " << DB_PATH << ")\n"
                          << "  --covers or -c    Path to covers folder (default " << COVER_PATH << ")\n";
                std::exit(1);
        }
    }
}
