cmake_minimum_required(VERSION 3.20)

project(Messenger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(messenger_core
    src/Socket.cpp
)

target_include_directories(
    messenger_core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(
    messenger_core
    PRIVATE

    -Wall
    -Wextra
    -Wpedantic
)

# собрать исполняемый файл
add_executable(
    Client
    src/Client.cpp
)

target_link_libraries(
    Client
    PRIVATE
    messenger_core
)

target_compile_options(
    Client
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_executable(
    Server
    src/Server.cpp
)

target_link_libraries(
    Server
    PRIVATE
    messenger_core
)

target_compile_options(
    Server
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_executable(
    SocketTest
    src/main.cpp
)

target_link_libraries(
    SocketTest
    PRIVATE
    messenger_core
)

target_compile_options(
    SocketTest
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
