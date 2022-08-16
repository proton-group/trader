cmake_minimum_required(VERSION 3.1.0)
project(test)

add_subdirectory(src)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(test test.cpp)
target_link_libraries(test exchange gtest)