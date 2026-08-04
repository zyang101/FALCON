add_executable(falcon_tests
    MatchingEngineTests.cpp
)

target_link_libraries(falcon_tests
    PRIVATE
        falcon_core
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(falcon_tests)
