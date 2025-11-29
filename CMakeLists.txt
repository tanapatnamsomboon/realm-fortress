cmake_minimum_required(VERSION 3.25)

project(realm-fortress
        VERSION 0.1.0
        LANGUAGES C CXX
        DESCRIPTION "Realm Fortress"
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

add_executable(realm-fortress
        src/core/entry_point.cpp

        # core
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

        # events
        src/events/application_event.h
        src/events/event.h
        src/events/key_event.h
        src/events/mouse_event.h

        # game
        src/game/application_impl.cpp
        src/game/building.cpp
        src/game/building.h
        src/game/game_layer.cpp
        src/game/game_layer.h
        src/game/hex_coordinate.cpp
        src/game/hex_coordinate.h
        src/game/hex_map.cpp
        src/game/hex_map.h
        src/game/resource_type.cpp
        src/game/resource_type.h
        src/game/selection.cpp
        src/game/selection.h

        # imgui
        src/imgui/imgui_layer.cpp
        src/imgui/imgui_layer.h

        # renderer
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
        src/renderer/render_command.cpp
        src/renderer/render_command.h
        src/renderer/renderer.cpp
        src/renderer/renderer.h
        src/renderer/renderer_api.h
        src/renderer/shader.cpp
        src/renderer/shader.h
        src/renderer/texture.cpp
        src/renderer/texture.h
        src/renderer/vertex_array.cpp
        src/renderer/vertex_array.h

        # scene
        src/scene/camera.cpp
        src/scene/camera.h
        src/scene/camera_controller.cpp
        src/scene/camera_controller.h
)

target_link_libraries(realm-fortress PRIVATE
        glfw
        glad
        glm::glm
        assimp::assimp
        imgui
        stb
        spdlog::spdlog
)

target_precompile_headers(realm-fortress PRIVATE
    src/core/pch.h
)

target_include_directories(realm-fortress PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_custom_command(TARGET realm-fortress POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:realm-fortress>/assets
)
