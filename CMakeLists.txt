cmake_minimum_required(VERSION 3.20)
project(Synth LANGUAGES CXX)

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
# Collect ALL .cpp files in examples/ — automatically updates when you add new ones
# ------------------------------------------------------------------
file(GLOB EXAMPLE_SOURCES
    CONFIGURE_DEPENDS                  # ← this is the magic
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp"
)

# ------------------------------------------------------------------
# Executable
# ------------------------------------------------------------------
add_executable(synth_demo ${EXAMPLE_SOURCES})

target_include_directories(synth_demo PRIVATE src)
target_link_libraries(synth_demo PRIVATE miniaudio_lib pthread dl)


