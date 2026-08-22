cmake_minimum_required(VERSION 3.20)

project(CnaExamples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# CNA dependency — consumed as a sibling add_subdirectory, so CNA's own CMake
# machinery (SDL vendoring, renderer
# selection, ...) is fully reused rather than duplicated here.
# ---------------------------------------------------------------------------

# OPENGLES3 is CNA's Linux default renderer and the successor to the old
# EASYGL selection. A preset or -DCNA_GRAPHICS_RENDERER=<X> on the command
# line still wins because CACHE variables set earlier on the command line are
# not overwritten here.
if(NOT DEFINED CNA_GRAPHICS_RENDERER)
    set(CNA_GRAPHICS_RENDERER "OPENGLES3" CACHE STRING
        "Graphics renderer to use (SDL_RENDERER, OPENGLES3, OPENGL33, VULKAN, WEBGPU, ...)")
endif()

set(CNA_BUILD_TESTS           OFF CACHE BOOL "Build CNA tests"           FORCE)
set(CNA_BUILD_EXAMPLES        OFF CACHE BOOL "Build CNA examples"        FORCE)
set(SHARP_RUNTIME_BUILD_TESTS OFF CACHE BOOL "Build sharp-runtime tests" FORCE)

# The Devices area (src/Demos/Devices/) demonstrates CNA::Devices::* (Camera,
# Clipboard, DisplayInfo, FileDialog, Locale, MessageBox, PowerInfo,
# SystemInfo, SystemTray, UrlLauncher) -- all compiled out entirely unless
# this is ON (default OFF in CNA itself, since these are CNA-specific
# extensions beyond XNA 4.0/WP7, not something every CNA consumer wants).
set(CNA_DEVICES ON CACHE BOOL "Enable CNA-specific device/sensor extensions" FORCE)

# CNA::Graphics::RenderPipelineSettings (D2's honesty screen) lives entirely
# behind this flag -- default OFF in CNA itself, same reasoning as
# CNA_DEVICES above (an extension beyond XNA 4.0).
set(CNA_CNAEXT ON CACHE BOOL "Enable CNA's extended graphics layer" FORCE)

# cnanext consumes sharp-runtime as a sibling checkout. Pin that consumed
# checkout explicitly so a build never silently falls back to ../sharp-runtime.
set(CNA_SHARP_RUNTIME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../sharp-runtimenext"
    CACHE PATH "Path to the sharp-runtime checkout consumed by CNA" FORCE)

add_subdirectory(../cnanext CNA_BUILD)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ExamplesHelpers)

# ---------------------------------------------------------------------------
# cna_examples — the single browsable demo-catalog application.
# See plan.md for the navigation architecture and Area/Category/Demo layout.
# ---------------------------------------------------------------------------

add_executable(cna_examples
    src/Program.cpp
)

target_include_directories(cna_examples PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Scoped to this target only -- CNA's own targets (added above via
# add_subdirectory) are not ours to warn about or fix here.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(cna_examples PRIVATE -Wall -Wextra)
endif()

cna_examples_configure_target(cna_examples)
