cmake_minimum_required(VERSION 3.20)

project(CardRoguelike VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(card_roguelike
    src/main.cpp
    src/game/Battle.cpp
    src/game/Card.cpp
    src/game/Player.cpp
    src/game/Enemy.cpp
)

target_include_directories(card_roguelike PRIVATE include)

if(MSVC)
    target_compile_options(card_roguelike PRIVATE /W4 /permissive-)
else()
    target_compile_options(card_roguelike PRIVATE -Wall -Wextra -Wpedantic)
endif()
