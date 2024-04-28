cmake_minimum_required(VERSION 3.20)
project(Minesweeper LANGUAGES CXX)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
)
FetchContent_MakeAvailable(SFML)

set(src_files
    src/Field.cpp
    src/Game.cpp
    src/main.cpp)

add_executable(Minesweeper ${src_files})
target_include_directories(Minesweeper PRIVATE include)
target_link_libraries(Minesweeper PRIVATE sfml-graphics sfml-window sfml-audio sfml-system)
target_compile_features(Minesweeper PRIVATE cxx_std_17)

INSTALL(TARGETS Minesweeper)