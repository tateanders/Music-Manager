cmake_minimum_required(VERSION 3.15)
project(Music-Manager C)

set(CMAKE_C_STANDARD 99)

# ---------------------------------------------------------
# Fetch Raylib automatically (no need for manual install)
# ---------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0  # You can change this to another version or branch
)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_MakeAvailable(raylib)

# Include directories for Raylib and Clay
include_directories(
    ${RAYLIB_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/clay
    ${CMAKE_CURRENT_SOURCE_DIR}/renderers/raylib
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_CURRENT_SOURCE_DIR}/frontend
    ${CMAKE_CURRENT_SOURCE_DIR}/backend
)

# Define sources
set(CLAY_SOURCES
    # main
    src/main.c
    # renderer/clay
    renderers/raylib/clay_renderer_raylib.h
    clay/clay.h
    # frontend
    frontend/front.c
    frontend/components.c
    frontend/globals.c
    # backend
    backend/data.c
    backend/duplicates.c
    backend/metadata.c
    backend/ID3v2dot3/editID3v2dot3.c
    backend/ID3v2dot3/readID3v2dot3.c
    backend/ID3v2dot4/editID3v2dot4.c
    backend/ID3v2dot4/readID3v2dot4.c
    backend/structures/dynarray.c
    backend/structures/list.c
)

# Create the executable
add_executable(Music-Manager ${CLAY_SOURCES})

# Link Raylib and macOS frameworks
target_link_libraries(Music-Manager
    raylib
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreAudio"
    "-framework CoreVideo"
)

add_custom_command(
    TARGET Music-Manager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_CURRENT_BINARY_DIR}/resources
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/*
    COMMENT "Cleaning all build files..."
)