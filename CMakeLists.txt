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

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG        glad2
        GIT_PROGRESS   TRUE
        SOURCE_SUBDIR  cmake
)
FetchContent_MakeAvailable(glad)
glad_add_library(glad STATIC REPRODUCIBLE LOADER API gl:core=4.6)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.2
)
FetchContent_MakeAvailable(glm)

target_link_libraries(RealmFortress
        glfw
        glad
        glm
)
