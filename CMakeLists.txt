cmake_minimum_required(VERSION 3.14)

# Project configuration
project(hw06)
file(GLOB SOURCES "src/*.c")
set(EXECUTABLE pandemic)

file(GLOB TESTS_DATA "tests/data")

# CUT Tests
file(GLOB TESTS_CUT_GLOB "tests/tests-cut/*.c")
file(GLOB TESTS_CUT_LIB_GLOB "tests/tests-cut/lib/*.c")

set(SOURCES_LIB ${SOURCES})
list(FILTER SOURCES_LIB EXCLUDE REGEX "/?main\\.c$")

foreach (file_path ${TESTS_CUT_GLOB})
    message("Found test: " ${file_path})
endforeach ()

set(TEST_SOURCES
    ${TESTS_CUT_LIB_GLOB}
    ${TESTS_CUT_GLOB}
    ${SOURCES_LIB}
)

add_definitions(-DCUT -DCUT_TIMEOUT=15 -DWRAP_INDIRECT)
add_executable(pandemic-tests-cut ${TEST_SOURCES})

# Symlink data files
file(CREATE_LINK ${TESTS_DATA} "${CMAKE_BINARY_DIR}/data" SYMBOLIC)

# Executable
add_executable(pandemic ${SOURCES})

# CLI Tests
file(GLOB TESTS_CLI_SH "tests/tests-cli/*.sh")
file(COPY ${TESTS_CLI_SH} "tests/tests-cli/pandemic-tests-cli" DESTINATION ${CMAKE_BINARY_DIR})

# Enable posix
target_compile_definitions(pandemic PUBLIC -D_POSIX_C_SOURCE=200809L)
target_compile_definitions(pandemic-tests-cut PUBLIC -D_POSIX_C_SOURCE=200809L)

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
