cmake_minimum_required(VERSION 3.14)
project(Apollonia VERSION 0.3.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

add_library(sqlite3 STATIC thirdparty/SQLiteCpp/sqlite3/sqlite3.c)
target_include_directories(sqlite3 PUBLIC thirdparty/SQLiteCpp/sqlite)
target_compile_definitions(sqlite3 PRIVATE -DSQLITE_ENABLE_FTS5)

set(SQLITECPP_INTERNAL_SQLITE OFF)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp)

set(DAO_PATH src/model/repository/dao/)
set(DAO_SOURCES ${DAO_PATH}/albums.cpp
    ${DAO_PATH}/lyrics.cpp
    ${DAO_PATH}/playlist_songs.cpp
    ${DAO_PATH}/playlists.cpp
    ${DAO_PATH}/songs_meta.cpp
    ${DAO_PATH}/songs.cpp
)

set(PROJECT_SOURCES src/model/repository/db_manager.cpp
    src/model/repository/apollo_repository.cpp
    src/model/file_service.cpp
    src/presenter/apollo_presenter.cpp
    src/view/routes.cpp
    src/main.cpp
)

include_directories(include
    include/presenter
    include/view
    include/model
    include/model/repository
    include/model/repository/dao
    thirdparty/sqlite
)

add_library(dao ${DAO_SOURCES})
target_link_libraries(dao SQLiteCpp sqlite3 pthread)

add_executable(Apollonia ${PROJECT_SOURCES})
target_link_libraries(Apollonia dao pthread)

add_executable(db_initializer 
    initer_src/fs.cpp
    initer_src/main.cpp
    src/model/repository/db_manager.cpp
)
target_link_libraries(db_initializer dao pthread avformat avutil)

enable_testing()

add_executable(db_tests 
    tests/dao/test_db_manager.cpp
    tests/dao/test_album_dao.cpp
    tests/dao/test_song_dao.cpp
    tests/dao/test_song_meta_dao.cpp
    tests/dao/test_lyrics_dao.cpp
    tests/dao/test_playlist_dao.cpp
    tests/dao/test_playlist_song_dao.cpp
    src/model/repository/db_manager.cpp
)

target_link_libraries(db_tests dao SQLiteCpp gtest gtest_main pthread)
add_test(NAME db_tests COMMAND db_tests)
