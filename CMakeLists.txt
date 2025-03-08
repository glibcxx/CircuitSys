cmake_minimum_required(VERSION 3.10.0)
project(BERS VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

file(GLOB_RECURSE native_srcs src/*.cpp)
add_executable(BERS main.cpp ${native_srcs})
