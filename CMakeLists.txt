cmake_minimum_required(VERSION 3.10)
project(codec LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(codec src/main.cpp)

if(MSVC)
    target_compile_options(codec PRIVATE /W4)
else()
    target_compile_options(codec PRIVATE -Wall -Wextra -Wpedantic)
endif()
