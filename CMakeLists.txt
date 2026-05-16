cmake_minimum_required(VERSION 3.20)

project(SnowEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)
find_package(Catch2 3 REQUIRED)

include_directories(
    include
    include/ui
    scripts/include
    blueprints/include
    scene_builders/include
    terminal/include
    snowlang/include
    snowlang/commands
)

file(GLOB_RECURSE ENGINE_SOURCES
    src/*.cpp
    scripts/src/*.cpp
    blueprints/src/*.cpp
    terminal/src/*.cpp
    scene_builders/src/*.cpp
    snowlang/src/*.cpp
    snowlang/commands/*.cpp
)

# Remove main.cpp from the library sources
list(REMOVE_ITEM ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

# =========================
# Engine Library
# =========================
add_library(snowlib OBJECT ${ENGINE_SOURCES})

target_link_libraries(snowlib
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

target_compile_options(snowlib PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -O2
)

# =========================
# Main Executable
# =========================
add_executable(snow
    src/main.cpp
    $<TARGET_OBJECTS:snowlib>
)

target_link_libraries(snow
    snowlib
)

# =========================
# Assets
# =========================
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY config DESTINATION ${CMAKE_BINARY_DIR})

# =========================
# Tests
# =========================
enable_testing()

add_subdirectory(tests)