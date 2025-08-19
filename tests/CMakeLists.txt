add_executable(Tests
        boardTests.cpp
        moveTests.cpp
        mainTests.cpp
        moveTests.cpp
        typeHelpersTests.cpp
        perftTests.cpp
)

target_include_directories(Tests PRIVATE ${CMAKE_SOURCE_DIR}/tests/include)

target_link_libraries(Tests PUBLIC Perft)

add_test(NAME AllUnitTests COMMAND Tests)