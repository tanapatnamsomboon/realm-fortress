cmake_minimum_required(VERSION 3.21)
project(RealmFortress LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(config/config.cmake)
include(config/compiler.cmake)
include(config/platform.cmake)

include(FetchContent)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

add_executable(RealmFortress ${SRC_FILES})

target_include_directories(RealmFortress PRIVATE
    src
)
