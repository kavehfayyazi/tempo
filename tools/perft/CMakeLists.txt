add_library(Perft INTERFACE)

target_include_directories(Perft INTERFACE ${CMAKE_SOURCE_DIR}/tools/perft)

target_link_libraries(Perft INTERFACE Board)