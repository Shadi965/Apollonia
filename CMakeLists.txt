cmake_minimum_required(VERSION 3.14)
project(Apollonia VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

set(DAO_PATH src/model/repository/dao/)
set(DAO_SOURCES ${DAO_PATH}/albums.cpp
                ${DAO_PATH}/lyrics.cpp
                ${DAO_PATH}/playlist_songs.cpp
                ${DAO_PATH}/playlists.cpp
                ${DAO_PATH}/songs_meta.cpp
                ${DAO_PATH}/songs.cpp)

include_directories(include
                    include/model
                    include/model/repository
                    include/model/repository/dao)

add_library(dao ${DAO_SOURCES})
add_library(fs src/model/file_service.cpp)
add_library(db_manager src/model/repository/db_manager.cpp)
add_library(song_repos src/model/repository/song_repository.cpp
                            src/model/repository/song_repository.cpp)


add_executable(Apollonia src/main.cpp)


# add_custom_command(TARGET Apollonia PRE_BUILD
#     COMMAND echo "Остановка контейнера"
#     COMMAND docker-compose down
# )

# add_custom_command(TARGET Apollonia POST_BUILD
#     COMMAND echo "Запуск контейнера с сервером"
#     COMMAND docker-compose up -d
# )

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp)

target_link_libraries(song_repos dao fs)

target_link_libraries(db_manager SQLiteCpp)

target_link_libraries(dao
                      SQLiteCpp
                    #   pthread
                      )

target_link_libraries(fs
                      avformat
                      avutil
                    #   pthread
                      )

target_link_libraries(Apollonia db_manager song_repos)


# target_link_libraries(Apollonia
#                       SQLiteCpp
#                       sqlite3
#                       avformat
#                       avutil
#                       pthread
#                       dl
#                       tag
#                       z)