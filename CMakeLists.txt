cmake_minimum_required(VERSION 3.25)

project(realm-fortress
        VERSION 0.1.0
        LANGUAGES C CXX
        DESCRIPTION "Realm Fortress - Medieval RTS Game"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(config/platform.cmake)
include(config/compiler.cmake)
include(config/precompile.cmake)
include(config/vendor.cmake)

# Source Files
set(REALM_FORTRESS_SOURCES
        # Entry Point
        src/core/entry_point.cpp

        # Core
        src/core/application.cpp
        src/core/application.h
        src/core/base.h
        src/core/input.cpp
        src/core/input.h
        src/core/key_codes.h
        src/core/layer.cpp
        src/core/layer.h
        src/core/layer_stack.cpp
        src/core/layer_stack.h
        src/core/logger.cpp
        src/core/logger.h
        src/core/mouse_codes.h
        src/core/timestep.h
        src/core/window.cpp
        src/core/window.h

        # Event
        src/events/application_event.h
        src/events/event.h
        src/events/key_event.h
        src/events/mouse_event.h

        # Renderer
        src/renderer/buffer.cpp
        src/renderer/buffer.h
        src/renderer/graphics_context.cpp
        src/renderer/graphics_context.h
        src/renderer/material.cpp
        src/renderer/material.h
        src/renderer/mesh.cpp
        src/renderer/mesh.h
        src/renderer/model.cpp
        src/renderer/model.h
        src/renderer/renderer.cpp
        src/renderer/renderer.h
        src/renderer/shader.cpp
        src/renderer/shader.h
        src/renderer/texture.cpp
        src/renderer/texture.h
        src/renderer/vertex_array.cpp
        src/renderer/vertex_array.h

        # Scene
        src/scene/camera.cpp
        src/scene/camera.h
        src/game/system/camera_controller.cpp
        src/game/system/camera_controller.h

        # ImGui
        src/imgui/imgui_layer.cpp
        src/imgui/imgui_layer.h

        # Game
        src/game/application_impl.cpp
        src/game/game_layer.cpp
        src/game/game_layer.h

        # Game - System
        src/game/system/constants.h
        src/game/system/coordinate.cpp
        src/game/system/coordinate.h
        src/game/system/map.cpp
        src/game/system/map.h
        src/game/system/picker.cpp
        src/game/system/picker.h
        src/game/system/selection.cpp
        src/game/system/selection.h
        src/game/system/tile.cpp
        src/game/system/tile.h
        src/game/system/utils.h

        # Game - Building

        # Game - Unit

        # Game - Resource

        # Game - Gameplay
)

add_executable(realm-fortress ${REALM_FORTRESS_SOURCES})

target_link_libraries(realm-fortress PRIVATE
        glad
        glfw
        glm
        imgui
        spdlog
        assimp
        stb
)

target_include_directories(realm-fortress PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(WIN32)
    target_compile_definitions(realm-fortress PRIVATE
            RF_PLATFORM_WINDOWS
            NOMINMAX
            WIN32_LEAN_AND_MEAN
    )
elseif(APPLE)
    target_compile_definitions(realm-fortress PRIVATE
            RF_PLATFORM_MACOS
    )
elseif(UNIX)
    target_compile_definitions(realm-fortress PRIVATE
            RF_PLATFORM_LINUX
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(realm-fortress PRIVATE
            RF_DEBUG
    )
else()
    target_compile_definitions(realm-fortress PRIVATE
            RF_RELEASE
            NDEBUG
    )
endif()

add_custom_command(TARGET realm-fortress POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:realm-fortress>/assets
        COMMENT "Copying assets to build directory"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${REALM_FORTRESS_SOURCES})

message(STATUS "===========================================")
message(STATUS "Realm Fortress Configuration")
message(STATUS "===========================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "===========================================")
