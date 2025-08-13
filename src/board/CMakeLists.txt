add_library(Board STATIC board.cpp
        types.h
        zobrist.cpp
        move.h
        typeHelpers.h
)

target_include_directories(Board PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#target_link_libraries(Board PRIVATE)