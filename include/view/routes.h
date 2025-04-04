#ifndef ROUTES_H
#define ROUTES_H

#include <crow.h>

#include "song_presenter.h"

class RoutesManager {
private:
    crow::SimpleApp app;

public:
    RoutesManager(uint16_t port, SongPresenter& songPresenter) {
        CROW_ROUTE(app, "/songs")([&songPresenter]() {
            return songPresenter.allSongsInfo();
        });
        
        CROW_ROUTE(app, "/song/<int>")([&songPresenter](int id) {
            return songPresenter.songInfo(id);
        });

        CROW_ROUTE(app, "/stream/<int>")([&songPresenter](const crow::request& req, int id){
            return songPresenter.streamSong(id, req);
        });

        CROW_ROUTE(app, "/download/<int>")([&songPresenter](int id){
            return songPresenter.downloadSong(id);
        });

        CROW_ROUTE(app, "/lyrics/<int>")([&songPresenter](int id){
            return songPresenter.songLyric(id);
        });

        CROW_ROUTE(app, "/meta/")([&songPresenter](){
            return songPresenter.songsMeta();
        });

        CROW_ROUTE(app, "/playlists/")([&songPresenter](){
            return songPresenter.allPlaylists();
        });

        CROW_ROUTE(app, "/playlist/<int>")([&songPresenter](int id){
            return songPresenter.getAllSongsFromPlaylist(id);
        });

        CROW_ROUTE(app, "/playlist/new/<string>")([&songPresenter](std::string name){
            songPresenter.createPlaylist(name);
            return "Ok";
        });

        CROW_ROUTE(app, "/playlist/update/<int>/<string>")([&songPresenter](int id, std::string newName){
            songPresenter.changePlaylistName(id, newName);
            return "Ok";
        });

        CROW_ROUTE(app, "/playlist/add/<int>/<int>")([&songPresenter](int id, int songId){
            songPresenter.addSongToPlaylist(id, songId);
            return "Ok";
        });

        CROW_ROUTE(app, "/playlist/remove/<int>/<int>")([&songPresenter](int id, int songId){
            songPresenter.removeSongFromPlaylist(id, songId);
            return "Ok";
        });

        CROW_ROUTE(app, "/playlist/delete/<int>")([&songPresenter](int id){
            songPresenter.deletePlaylist(id);
            return "Ok";
        });

        app.port(port).multithreaded().run();
    }
};


#endif // ROUTES_H
