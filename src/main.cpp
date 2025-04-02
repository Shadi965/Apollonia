#include <iostream>

#include "db_manager.h"
#include "song_repository.h"

int main() {
    DatabaseManager dbManager("./songs.db");
    std::cout << "База данных инициализирована" << std::endl;
    SongRepository songRepository(dbManager.getDb());
    std::cout << "Репозиторий инициализирован" << std::endl;
    songRepository.initDB("/mnt/c/Users/Shadi/Music/Apple Music/Media/Music", 
                            "/mnt/c/Users/Shadi/Downloads/Lyrics/json");
    std::cout << "База данных наполнена" << std::endl;
    return 0;
}