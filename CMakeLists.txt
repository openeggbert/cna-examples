cmake_minimum_required(VERSION 3.20)

project(CnaExamples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# CNA dependency — consumed as a sibling add_subdirectory, same as
# ../cna-samples, so CNA's own CMake machinery (SDL vendoring, backend
# selection, ...) is fully reused rather than duplicated here.
# ---------------------------------------------------------------------------

# EasyGL is CNA's strongest backend today (see plan.md section 3); a preset
# or -DCNA_GRAPHICS_BACKEND=<X> on the command line still wins because CACHE
# variables set earlier on the command line are not overwritten here.
if(NOT DEFINED CNA_GRAPHICS_BACKEND)
    set(CNA_GRAPHICS_BACKEND "EASYGL" CACHE STRING
        "Graphics backend to use (SDL_RENDERER, EASYGL, BGFX, VULKAN, WEBGPU, ...)")
endif()

set(CNA_BUILD_TESTS           OFF CACHE BOOL "Build CNA tests"           FORCE)
set(CNA_BUILD_EXAMPLES        OFF CACHE BOOL "Build CNA examples"        FORCE)
set(SHARP_RUNTIME_BUILD_TESTS OFF CACHE BOOL "Build sharp-runtime tests" FORCE)

add_subdirectory(../cna CNA_BUILD)

# Re-export the backend target name CNA sets internally, for the linker
# group in ExamplesHelpers.cmake. Falls back gracefully if CNA doesn't
# export it (no linker group — a normal link is attempted instead).
if(NOT DEFINED BACKEND_TARGET)
    if(CNA_GRAPHICS_BACKEND STREQUAL "EASYGL")
        set(BACKEND_TARGET "easy-gl")
    elseif(CNA_GRAPHICS_BACKEND STREQUAL "VULKAN")
        set(BACKEND_TARGET "cna_backend_graphics_vulkan")
    endif()
endif()

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

cna_examples_configure_target(cna_examples)
