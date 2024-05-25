set(TARGET chat_server)

file(GLOB_RECURSE SRC "src/*.cpp")
add_executable(${TARGET} ${SRC})
target_include_directories(${TARGET}
    PRIVATE include
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(${TARGET} PRIVATE wheel)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/chat_server.json
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
