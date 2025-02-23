cmake_minimum_required(VERSION 3.16)

option(BUILD_WHEEL_TEST "build wheel test" OFF)
option(BUILD_ECHO_SERVER "build echo server" OFF)
option(BUILD_CHAT_SERVER "build chat server" OFF)
# option(BUILD_MYSQL "build mysql" OFF)

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
    DESCRIPTION "my personal cpp wheels"
    HOMEPAGE_URL "https://github.com/m1dsolo/wheel"
    LANGUAGES C CXX
)

# build wheel
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(${TARGET} OBJECT ${SRC})
target_include_directories(${TARGET}
    PUBLIC include
)

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

# use to tmp test
add_executable(wheel_tmp main.cpp)
target_link_libraries(wheel_tmp wheel)
target_include_directories(wheel_tmp PRIVATE include)
