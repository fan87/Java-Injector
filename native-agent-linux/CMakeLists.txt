cmake_minimum_required(VERSION 3.24)
project(native_agent_linux)

set(CMAKE_CXX_STANDARD 17)

add_library(native_agent_linux SHARED src/main/cpp/me_fan87_javainjector_NativeInstrumentation.cpp)

include_directories(/lib/jvm/default/include /lib/jvm/default/include/linux libs/)
