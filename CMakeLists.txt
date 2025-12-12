cmake_minimum_required(VERSION 3.20)
project(PatchMagicSynth LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------
# Fetch miniaudio (single-header, header-only)
# ------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG        0.11.21
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(miniaudio)

add_library(miniaudio_lib INTERFACE)
target_include_directories(miniaudio_lib INTERFACE ${miniaudio_SOURCE_DIR})

add_compile_definitions(
    MA_NO_DECODING
    MA_NO_ENCODING
    MA_NO_GENERATION
)

# ------------------------------------------------------------------
# patch_magic static library
# Automatically includes all .cpp files in src/ (recursively if needed)
# ------------------------------------------------------------------
file(GLOB PATCH_MAGIC_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(patch_magic STATIC ${PATCH_MAGIC_SOURCES})

target_include_directories(patch_magic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(patch_magic PUBLIC miniaudio_lib)

# Enable warnings and strict compilation for the library
target_compile_options(patch_magic PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# ------------------------------------------------------------------
# Collect all example sources (automatically updates when new examples are added)
# ------------------------------------------------------------------
file(GLOB EXAMPLE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp"
)

# ------------------------------------------------------------------
# Main executable: synth_demo
# ------------------------------------------------------------------
add_executable(synth_demo ${EXAMPLE_SOURCES})

target_link_libraries(synth_demo PRIVATE patch_magic)

# Ensure examples can see the public headers
target_include_directories(synth_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Platform-specific libraries (Linux needs pthread and dl)
if(UNIX AND NOT APPLE)
    target_link_libraries(synth_demo PRIVATE pthread dl)
endif()


