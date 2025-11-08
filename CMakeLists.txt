cmake_minimum_required(VERSION 3.21)
project(RealmFortress LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(config/config.cmake)
include(config/compiler.cmake)
include(config/platform.cmake)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

add_executable(RealmFortress ${SRC_FILES})

target_include_directories(RealmFortress PRIVATE
        src
)

include(FetchContent)

# glfw
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# glad
FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG glad2
        GIT_PROGRESS TRUE
        SOURCE_SUBDIR cmake
)
FetchContent_MakeAvailable(glad)
glad_add_library(glad STATIC REPRODUCIBLE LOADER API gl:core=4.6)

# glm
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.2
)
FetchContent_MakeAvailable(glm)

# imgui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_link_libraries(imgui
        PRIVATE
        glfw
        glad
)
target_include_directories(imgui
        PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

# assimp
FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp
        GIT_TAG master
)
FetchContent_MakeAvailable(assimp)

target_link_libraries(RealmFortress
        glfw
        glad
        glm
        imgui
)

add_custom_command(TARGET RealmFortress POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:RealmFortress>/assets/shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets/shaders
            $<TARGET_FILE_DIR:RealmFortress>/assets/shaders
)
