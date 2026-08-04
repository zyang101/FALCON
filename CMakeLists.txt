cmake_minimum_required(VERSION 3.20)

project(Falcon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Shared exchange/matching sources (used by app + tests)
add_library(falcon_core
    src/common/Time.cpp
    src/orderbook/LimitOrderBook.cpp
    src/matching/MatchingEngine.cpp
)

target_include_directories(falcon_core PUBLIC include)

add_executable(falcon src/main.cpp)
target_link_libraries(falcon PRIVATE falcon_core)

# Tests
option(FALCON_BUILD_TESTS "Build unit tests" ON)

if(FALCON_BUILD_TESTS)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
    )
    # Prevent GoogleTest from overriding parent compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()
