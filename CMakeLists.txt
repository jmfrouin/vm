cmake_minimum_required(VERSION 3.20)
project(VM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)
include_directories(src/cpu)
include_directories(src/memory)
include_directories(src/io)
include_directories(src/storage)

# Source files
file(GLOB_RECURSE SOURCES
        "src/*.cpp"
        "src/*.h"
)

# Main executable
add_executable(VM main.cpp ${SOURCES})

# Enable debug symbols and warnings
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Tests (optional)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
