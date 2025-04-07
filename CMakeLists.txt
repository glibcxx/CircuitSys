cmake_minimum_required(VERSION 3.30.0)
project(BERS VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /source-charset:utf-8 /execution-charset:utf-8 /wd4244 /wd4267")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8 -fexec-charset=UTF-8")
endif()

add_subdirectory(external/GSL)

include_directories(src)

file(GLOB_RECURSE native_srcs src/*.cpp)
add_executable(BERS main.cpp ${native_srcs})

target_link_libraries(BERS PRIVATE GSL)
