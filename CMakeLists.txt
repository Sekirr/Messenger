cmake_minimum_required(VERSION 3.20)
project(Messenger)
set(CMAKE_CXX_STANDARD 20)
add_executable(
    MessengerClient
    src/client/main.cpp
)
add_executable(
    MessengerServer
    src/server/main.cpp
)
target_include_directories(
    MessengerClient
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(
    MessengerServer
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
