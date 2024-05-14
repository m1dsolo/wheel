cmake_minimum_required(VERSION 3.20)

option(BUILD_TEST "build test" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
find_program(CLANGPP NAMES clang++)
if (CLANGPP)
    set(CMAKE_CXX_COMPILER clang++)
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "No build type selected, default to Release")
endif()

project(
    wheel
    VERSION 0.1.0
    DESCRIPTION "my personal cpp wheels"
    HOMEPAGE_URL "https://github.com/m1dsolo/wheel"
    LANGUAGES CXX
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# build wheel
set(TARGET wheel)
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(${TARGET} STATIC ${SRC})
target_include_directories(${TARGET} PUBLIC include)

# build test
if (BUILD_TEST)
    set(CMAKE_CXX_STANDARD 17)
    add_subdirectory(test)
endif()

# add_executable(main main.cpp)
# target_link_libraries(main wheel)
# target_include_directories(main PRIVATE include)
