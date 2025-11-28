# Compiler-specific flags and warnings

if (MSVC)
    # MSVC compiler flags
    add_compile_options(
            /W4                 # Warning level 4
            /WX                 # Warnings as errors
            /permissive-        # Standards conformance
            /Zc:__cplusplus     # Correct __cplusplus macro
            /Zc:preprocessor    # Standards-conforming preprocessor
            /MP                 # Multi-processor compilation
            /EHsc               # Exception handling model
    )

    # Disable specific warnings
    add_compile_options(
            /wd4100     # Unreferenced formal parameter
            /wd4201     # Nameless struct/union
    )

    # Release optimizations
    add_compile_options($<$<CONFIG:Release>:/O2>)
    add_link_options($<$<CONFIG:Release>:/LTCG>)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # GCC/Clang compiler flags
    add_compile_options(
            -Wall
            -Wextra
            -Wpedantic
            # -Werror
            # -Wconversion
            # -Wsign-conversion
            -Wno-unused-parameter
    )

    # Release optimizations
    add_compile_options($<$<CONFIG:Release>:-O3>)

    # Debug symbols
    add_compile_options($<$<CONFIG:Debug>:-g>)
endif ()

add_compile_definitions(
        $<$<CONFIG:Debug>:RF_DEBUG>
        $<$<CONFIG:Release>:RF_RELEASE>
        $<$<CONFIG:RelWithDebInfo>:RF_RELEASE>
        $<$<CONFIG:MinSizeRel>:RF_RELEASE>
)
