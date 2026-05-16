file(GLOB_RECURSE TEST_SOURCES *.cpp)

add_executable(engine_tests
    ${TEST_SOURCES}
)

target_link_libraries(engine_tests
    Catch2::Catch2WithMain
    snowlib
)

include(CTest)
include(Catch)

catch_discover_tests(engine_tests)