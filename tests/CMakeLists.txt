add_executable(Tests
        boardTests.cpp
        mainTests.cpp
)

target_include_directories(Tests PRIVATE ${CMAKE_SOURCE_DIR}/tests/include)

target_link_libraries(Tests PUBLIC Board)

add_test(NAME AllUnitTests COMMAND Tests)