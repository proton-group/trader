cmake_minimum_required(VERSION 3.1.0)
project(exchange)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(exchange exchange.cpp trader.cpp cotersParser.cpp financial_structurs.cpp)