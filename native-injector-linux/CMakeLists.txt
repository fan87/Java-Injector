cmake_minimum_required(VERSION 3.24)
project(native_injector_linux)

set(CMAKE_CXX_STANDARD 17)

add_library(native_injector_linux SHARED src/main/cpp/me_fan87_javainjector_JavaInjector.cpp)

include_directories(/lib/jvm/default/include /lib/jvm/default/include/linux libs/)
target_link_libraries(native_injector_linux ${CMAKE_SOURCE_DIR}/libs/libinjector.a)