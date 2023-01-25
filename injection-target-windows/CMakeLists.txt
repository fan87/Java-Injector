cmake_minimum_required(VERSION 3.24)
project(injection_target_windows)

set(CMAKE_CXX_STANDARD 14)

include_directories(src/main/public ../../include)

add_executable(injection_target_windows
        src/main/cpp/dllmain.cpp
        src/main/public/dllmain.h)
