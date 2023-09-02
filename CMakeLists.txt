cmake_minimum_required(VERSION 3.20)

project(web_api)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS system thread)

add_executable(web_api main.cpp)

target_link_libraries(web_api PRIVATE Boost::system Boost::thread)