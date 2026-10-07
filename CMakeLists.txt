cmake_minimum_required(VERSION 3.20)

project(Messenger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(messenger_core
    src/Socket.cpp
    src/TCPListener.cpp
    src/TCPConnection.cpp
)

target_include_directories(messenger_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(messenger_core
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

add_executable(messenger
    src/main.cpp
)

target_link_libraries(messenger
    PRIVATE
        messenger_core
)
