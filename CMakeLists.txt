cmake_minimum_required(VERSION 3.20)
project(2dengine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Mac-specific: add Homebrew paths
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH /opt/homebrew)
endif()

# Find packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)

# Collect sources
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include dirs
target_include_directories(${PROJECT_NAME} PRIVATE
    ./libs
    ./libs/spdlog/include
    ${SDL3_INCLUDE_DIRS}
)

# Link libraries
target_link_directories(${PROJECT_NAME} PRIVATE ${SDL3_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL3_LIBRARIES}
    SDL3_image
    SDL3_ttf
    SDL3_mixer
    lua
)

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wfatal-errors)
