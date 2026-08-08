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

# ImGui sources (SDL3 + SDL_Renderer backend)
set(IMGUI_SOURCES
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_impl_sdl3.cpp
    libs/imgui/imgui_impl_sdlrenderer3.cpp
        src/systems/RenderGUISystem.h
        src/game/LevelLoader.h
        src/game/LevelLoader.cpp
        src/components/ScriptComponent.h
        src/systems/ScriptSystem.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES}
        src/components/ProjectileEmitterComponent.h
        src/systems/ProjectileEmitSystem.h
        src/components/HealthComponent.h
        src/components/ProjectileComponent.h
        src/systems/ProjectileLifecycleSystem.h
        src/components/TextLabelComponent.h
        src/systems/RenderTextSystem.h
        src/systems/HealthBarRenderSystem.h)

# Include dirs
target_include_directories(${PROJECT_NAME} PRIVATE
    ./libs
    ./libs/spdlog/include
    ${SDL3_INCLUDE_DIRS}
)

# Link libraries
target_link_directories(${PROJECT_NAME} PRIVATE ${SDL3_LIBRARY_DIRS})
if(APPLE)
    target_link_directories(${PROJECT_NAME} PRIVATE /opt/homebrew/lib)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL3_LIBRARIES}
    SDL3_image
    SDL3_ttf
    SDL3_mixer
    lua
)

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wfatal-errors)
