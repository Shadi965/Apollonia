#define CROW_MAIN

#include "routes.h"
#include "apollo_presenter.h"
#include "apollo_repository.h"
#include "db_manager.h"

#define PORT 8080
#define DB_PATH "./songs.db"

int main() {
    DatabaseManager dbManager(DB_PATH);
    ApolloRepository repository(dbManager.getDb());

    ApolloPresenter presenter(repository, repository, repository);

    RoutesManager routesManager(PORT);

    routesManager.regSongRoutes(presenter);
    routesManager.regAlbumRoutes(presenter);
    routesManager.regPlaylistRoutes(presenter);

    routesManager.runApp();

    return 0;
}