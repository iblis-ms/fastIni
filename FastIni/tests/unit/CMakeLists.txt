CMake_minimum_required(VERSION 3.5.2 FATAL_ERROR)

include(conanbuildinfo.cmake)
conan_basic_setup()

set(TARGET_NAME FastIniUnitTest)
set(SRC src/FastIniUnitTest.cpp)

add_executable(${TARGET_NAME} ${SRC})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 14)

find_package(Threads)

target_link_libraries(${TARGET_NAME} PRIVATE FastIni ${CONAN_LIBS} ${CMAKE_THREAD_LIBS_INIT})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(${TARGET_NAME} PUBLIC "-stdlib=libc++"  "-Wpedantic" "-Wall" "-Werror")
    set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "-stdlib=libc++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(${TARGET_NAME} PUBLIC "-D_GLIBCXX_USE_CXX11_ABI=1" "-Wpedantic" "-Wall" "-Werror")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(${TARGET_NAME} PUBLIC  "/MT" "-Wpedantic" "-Wall" "-Werror")
endif ()

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})

