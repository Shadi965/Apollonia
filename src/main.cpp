#define CROW_MAIN

#include <iostream>

#include "db_manager.h"
#include "file_service.h"
#include "song_repository.h"
#include "song_presenter.h"
#include "routes.h"

#define PORT 8080
#define DB_PATH "./songs.db"

int main() {
    DatabaseManager dbManager(DB_PATH);
    SongRepository songRepository(dbManager.getDb());

    FileService fileService;

    SongPresenter presenter(songRepository, fileService);

    RoutesManager routesManager(PORT, presenter);

    // songRepository.initDB("/mnt/c/Users/Shadi/Music/Apple Music/Media/Music", 
    //                         "/mnt/c/Users/Shadi/Downloads/Lyrics/json");
    // std::cout << "База данных наполнена" << std::endl;
    return 0;
}