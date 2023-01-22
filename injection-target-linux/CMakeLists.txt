cmake_minimum_required(VERSION 3.24)
project(injection_target_linux)

set(CMAKE_CXX_STANDARD 17)

add_library(injection_target_linux SHARED src/main/cpp/somain.cpp)

include_directories(/lib/jvm/default/include /lib/jvm/default/include/linux libs/)
