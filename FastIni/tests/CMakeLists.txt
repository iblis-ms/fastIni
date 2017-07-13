CMake_minimum_required(VERSION 3.5.2 FATAL_ERROR) 

project(FastIniTests VERSION 1.0.0 LANGUAGES CXX)

enable_testing()

add_subdirectory(unit)
add_subdirectory(benchmark)
