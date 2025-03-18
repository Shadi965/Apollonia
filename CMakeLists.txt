cmake_minimum_required(VERSION 3.14)
project(Apollonia VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(include)
add_executable(Apollonia src/main.cpp
src/database.cpp
src/api.cpp)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp)

target_link_libraries(Apollonia
                      SQLiteCpp
                      sqlite3
                      pthread
                      dl
                      tag
                      z)