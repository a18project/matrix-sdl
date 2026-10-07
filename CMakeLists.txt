cmake_minimum_required(VERSION 3.16)
project(matrix-sdl C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

set(SOURCES
    src/main.c
    src/matrix.c
    src/renderer.c
)

add_executable(matrix-sdl ${SOURCES})

target_include_directories(matrix-sdl PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(matrix-sdl PRIVATE
    ${SDL2_LIBRARIES}
    m
)

# Copy assets to build directory for easy execution
add_custom_command(TARGET matrix-sdl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:matrix-sdl>/assets
)

# Installation target
install(TARGETS matrix-sdl DESTINATION bin)
install(DIRECTORY assets DESTINATION share/matrix-sdl)
