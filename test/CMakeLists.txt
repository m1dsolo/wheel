set(TARGET_TEST wheel_test)
set(GTEST_ROOT ${PROJECT_SOURCE_DIR}/third_party/googletest)

add_subdirectory(${GTEST_ROOT} googletest.out)

enable_testing()

file(GLOB_RECURSE SRC "*.cpp")
add_executable(${TARGET_TEST} ${SRC})
target_include_directories(${TARGET}
    PRIVATE ${GTEST_ROOT}/include
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(${TARGET_TEST}
    PRIVATE GTest::gtest_main
    PRIVATE wheel
)

add_test(NAME ${TARGET_TEST} COMMAND ${TARGET_TEST})
