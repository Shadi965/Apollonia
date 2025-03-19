#ifndef API_H
#define API_H

#include <crow.h>
#include "database.h"

void setupRoutes(crow::SimpleApp& app, Database& db);

#endif // API_H
