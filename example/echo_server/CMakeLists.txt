set(TARGET echo_server)

file(GLOB_RECURSE SRC "src/*.cpp")
add_executable(${TARGET} ${SRC})
target_include_directories(${TARGET}
    PRIVATE include
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(${TARGET} PRIVATE wheel)

