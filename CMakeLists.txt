cmake_minimum_required(VERSION 3.20)

project(CardRoguelike VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(card_roguelike
    src/main.cpp
    src/ui/GameApp.cpp
    src/game/Battle.cpp
    src/game/Card.cpp
    src/game/GameSession.cpp
    src/game/Player.cpp
    src/game/Enemy.cpp
)

target_include_directories(card_roguelike PRIVATE include)

# MSYS2's MinGW toolchains install raylib below the same prefix as the compiler.
# This keeps the project configuration-free when CMake is invoked with that
# toolchain, while still allowing another installation to be supplied through
# CMAKE_PREFIX_PATH or raylib_DIR.
if(MINGW AND NOT DEFINED raylib_DIR)
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    get_filename_component(MINGW_PREFIX "${MINGW_BIN_DIR}" DIRECTORY)
    list(PREPEND CMAKE_PREFIX_PATH "${MINGW_PREFIX}")
endif()

find_package(raylib CONFIG REQUIRED)
target_link_libraries(card_roguelike PRIVATE raylib)

if(MSVC)
    target_compile_options(card_roguelike PRIVATE /W4 /permissive-)
else()
    target_compile_options(card_roguelike PRIVATE -Wall -Wextra -Wpedantic)
endif()
