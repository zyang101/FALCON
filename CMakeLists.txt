cmake_minimum_required(VERSION 3.20)

project(Falcon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(
    falcon
    src/main.cpp
    src/common/Time.cpp
    src/orderbook/LimitOrderBook.cpp
    src/matching/MatchingEngine.cpp
)