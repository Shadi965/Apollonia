#ifndef API_H
#define API_H

#include <crow.h>
#include "database.h"

void setupRoutes(crow::SimpleApp& app, Database& db);
void handleStream(const crow::request& req, crow::response& res, Database& db, int id);
void handleDownload(const crow::request& req, crow::response& res, Database& db, int id);

#endif // API_H
