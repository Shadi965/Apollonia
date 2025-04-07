cmake_minimum_required(VERSION 3.14)
project(Apollonia VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)



# add_custom_command(TARGET Apollonia PRE_BUILD
#     COMMAND echo "Остановка контейнера"
#     COMMAND docker-compose down
# )

# add_custom_command(TARGET Apollonia POST_BUILD
#     COMMAND echo "Запуск контейнера с сервером"
#     COMMAND docker-compose up -d
# )

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp)




set(DAO_PATH src/model/repository/dao/)
set(DAO_SOURCES ${DAO_PATH}/albums.cpp
                ${DAO_PATH}/lyrics.cpp
                ${DAO_PATH}/playlist_songs.cpp
                ${DAO_PATH}/playlists.cpp
                ${DAO_PATH}/songs_meta.cpp
                ${DAO_PATH}/songs.cpp)

set(PROJECT_SOURCES src/model/repository/db_manager.cpp
                    src/model/repository/apollo_repository.cpp
                    src/presenter/apollo_presenter.cpp
                    src/view/routes.cpp
                    src/main.cpp)



include_directories(include
                    include/presenter
                    include/view
                    include/model
                    include/model/repository
                    include/model/repository/dao)




add_library(dao ${DAO_SOURCES})
target_link_libraries(dao
                      SQLiteCpp
                      pthread
                      )



add_executable(Apollonia ${PROJECT_SOURCES})
target_link_libraries(Apollonia dao pthread)

# target_link_libraries(Apollonia
#                       SQLiteCpp
#                       sqlite3
#                       avformat
#                       avutil
#                       pthread
#                       dl
#                       tag
#                       z)