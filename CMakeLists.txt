cmake_minimum_required(VERSION 3.20)
project(plazza LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(SOURCES
    src/main.cpp
    src/Core/Process.cpp
    src/Core/Pizza.cpp
    src/Core/Thread.cpp
    src/Kitchen/Cook.cpp
)

add_executable(plazza ${SOURCES})

target_include_directories(plazza PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

find_package(Threads REQUIRED)

target_link_libraries(plazza PRIVATE Threads::Threads)

target_compile_options(plazza PRIVATE
    -Wall
    -Wextra
    -Werror
)
