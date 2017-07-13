CMake_minimum_required(VERSION 3.5.2 FATAL_ERROR)

project(FastIni VERSION 1.0.0 LANGUAGES C)

set(SRC "src/FastIni.c")

add_library(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC "inc")

target_compile_options(${PROJECT_NAME} PUBLIC "-Wpedantic" "-Wall" "-Werror")

