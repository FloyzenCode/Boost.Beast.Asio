cmake_minimum_required(VERSION 3.20)

project(http_server)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS system thread)

add_executable(http_server main.cpp)

target_link_libraries(http_server PRIVATE Boost::system Boost::thread)
