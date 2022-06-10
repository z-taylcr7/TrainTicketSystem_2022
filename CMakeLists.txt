cmake_minimum_required(VERSION 3.16)
project(src)

set(CMAKE_CXX_FLAGS "-std=c++17 -O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -Wall -fsanitize=address -fsanitize=undefined")
set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(code  main.cpp)

