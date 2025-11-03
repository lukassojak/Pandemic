cmake_minimum_required(VERSION 3.14)

# Project configuration
project(hw06)
file(GLOB SOURCES "src/*.c")
set(EXECUTABLE pandemic)

file(GLOB TESTS_DATA "tests/data")


# Executable
add_executable(pandemic ${SOURCES})

# Enable posix
target_compile_definitions(pandemic PUBLIC -D_POSIX_C_SOURCE=200809L)

# -------------------------------------------------------------------
# DO NOT EDIT LINES BELOW lest you break it

# Configure compiler warnings
if (CMAKE_C_COMPILER_ID MATCHES Clang OR ${CMAKE_C_COMPILER_ID} STREQUAL GNU)
    # using regular Clang, AppleClang or GCC
    # Strongly suggested: neable -Werror
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -pedantic -g")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
    # using Visual Studio C++
    target_compile_definitions(${EXECUTABLE} PRIVATE _CRT_SECURE_NO_DEPRECATE)
    target_compile_definitions(${EXECUTABLE_TESTS} PRIVATE _CRT_SECURE_NO_DEPRECATE)
    set(CMAKE_CXX_FLAGS "/permissive- /W4 /EHsc")
endif()

if(MINGW)
    target_compile_definitions(${EXECUTABLE} PRIVATE __USE_MINGW_ANSI_STDIO=1)
    target_compile_definitions(${EXECUTABLE_TESTS} PRIVATE _CRT_SECURE_NO_DEPRECATE)
endif()
