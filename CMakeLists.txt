cmake_minimum_required(VERSION 3.16)

option(BUILD_WHEEL_TEST "build wheel test" OFF)

set(TARGET wheel)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# default release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "No build type selected, default to Release")
endif()

project(
    wheel
    VERSION 0.1.0
    DESCRIPTION "cpp wheels"
    HOMEPAGE_URL "https://github.com/m1dsolo/wheel"
    LANGUAGES C CXX
)

# build wheel
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "src/*")
add_library(${TARGET} OBJECT ${SRC})
target_include_directories(${TARGET}
    PUBLIC include
)

# build test
if (BUILD_WHEEL_TEST)
    add_subdirectory(test)
endif()
