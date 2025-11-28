# Platform-specific settings

if (WIN32)
    # Windows-specific settings
    add_compile_definitions(RF_PLATFORM_WINDOWS)
    add_compile_definitions(NOMINMAX)               # Disable min/max macros
    add_compile_definitions(UNICODE _UNICODE)       # Unicode support
elseif (APPLE)
    # macOS-specific settings
    add_compile_definitions(RF_PLATFORM_MACOS)

    # Required frameworks
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
elseif (UNIX)
    # Linux-specific settings
    add_compile_definitions(RF_PLATFORM_LINUX)

    # Required libraries
    find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)
endif ()
