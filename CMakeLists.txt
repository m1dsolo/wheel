cmake_minimum_required(VERSION 3.20)

option(BUILD_WHEEL_TEST "build wheel test" OFF)
option(BUILD_ECHO_SERVER "build echo server" OFF)
option(BUILD_CHAT_SERVER "build chat server" OFF)
# option(BUILD_MYSQL "build mysql" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
find_program(CLANGPP NAMES clang++)
if (CLANGPP)
    # 打印clang版本
    execute_process(COMMAND ${CLANGPP} --version OUTPUT_VARIABLE CLANG_VERSION)
    message(STATUS "Found clang++: ${CLANGPP} (${CLANG_VERSION})")

    # maybe need version of clang++ >= 16
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-enum-constexpr-conversion")
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
    LANGUAGES C CXX
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# build wheel
set(TARGET wheel)
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(${TARGET} STATIC ${SRC})
target_include_directories(${TARGET} PUBLIC include)

# build test
if (BUILD_WHEEL_TEST)
    add_subdirectory(test)
endif()

# build echo server
if (BUILD_ECHO_SERVER)
    add_subdirectory(example/echo_server)
endif()

# build chat server
if (BUILD_CHAT_SERVER)
    add_subdirectory(example/chat_server)
endif()

# build mysql connector
# if (BUILD_MYSQL)
#     find_package(MySQL REQUIRED)
#     include_directories(${MYSQL_INCLUDE_DIRS})
#     # target_link_libraries(your_target_name ${MYSQL_LIBRARIES})
# endif()

# use to test
# add_executable(main main.cpp)
# target_link_libraries(main wheel)
# target_include_directories(main PRIVATE include)
