cmake_minimum_required(VERSION 3.10.0)
project(BERS VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
include(FetchContent)

FetchContent_Declare(
    GSL
    GIT_REPOSITORY https://github.com/microsoft/GSL.git
    GIT_TAG v4.0.0
)
FetchContent_MakeAvailable(GSL)

include_directories(src)

file(GLOB_RECURSE native_srcs src/*.cpp)
add_executable(BERS main.cpp ${native_srcs})

target_link_libraries(BERS PRIVATE GSL)
