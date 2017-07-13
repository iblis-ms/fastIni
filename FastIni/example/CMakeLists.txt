CMake_minimum_required(VERSION 3.5.2 FATAL_ERROR)

project(FastIniExample LANGUAGES C)

set(TARGET_NAME FastIniExample)

set(SRC "src/main.c")

add_executable(${TARGET_NAME} ${SRC})

target_link_libraries(${TARGET_NAME} PRIVATE FastIni)
target_compile_options(${TARGET_NAME} PUBLIC "-Wpedantic" "-Wall" "-Werror")

add_custom_command(
    TARGET ${TARGET_NAME} 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test.ini  ${CMAKE_BINARY_DIR}/example/test.ini
)
