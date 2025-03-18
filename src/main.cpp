#define CROW_MAIN
#include "crow.h"
#include "database.h"
#include "api.h"

#define PORT 8080

int main() {
    crow::SimpleApp app;
    Database db("songs.db");

    setupRoutes(app, db);

    app.port(PORT).multithreaded().run();
}