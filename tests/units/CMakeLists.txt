add_executable(
    demo_test
    ${AREG_UNIT_TEST_BASE}/DemoTest.cpp
)
target_link_libraries(
    demo_test
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(demo_test)