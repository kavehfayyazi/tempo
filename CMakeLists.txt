cmake_minimum_required(VERSION 3.31)
project(Tempo)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src/board)
add_subdirectory(Tests)
add_subdirectory(Tools)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Perft)