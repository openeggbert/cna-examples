# cmake/ExamplesHelpers.cmake
# Helper for configuring the cna-examples executable target(s). Isolates the
# platform-specific linking/packaging boilerplate (Linux linker group for the
# circular CNA/backend reference, Emscripten .html output, Windows SDL DLL
# copy) from the top-level CMakeLists.txt, mirroring
# ../cna-samples/cmake/SampleHelpers.cmake's cna_add_sample().

# cna_examples_configure_target(target_name)
#
# Links `target_name` against CNA/SHARP_RUNTIME (with a linker group on
# GCC/Clang Linux to resolve the circular CNA <-> graphics-backend
# reference), applies Emscripten/Windows packaging specifics, and copies
# Content/ next to the built executable.
function(cna_examples_configure_target target_name)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT WIN32 AND NOT EMSCRIPTEN)
        if(DEFINED BACKEND_TARGET AND TARGET ${BACKEND_TARGET})
            target_link_libraries(${target_name} PRIVATE
                -Wl,--start-group CNA ${BACKEND_TARGET} -Wl,--end-group
                SHARP_RUNTIME)
        else()
            target_link_libraries(${target_name} PRIVATE CNA SHARP_RUNTIME)
        endif()
    elseif(EMSCRIPTEN)
        # Deliberately does NOT name an SDL3 target. CNA imports SDL3 via
        # find_package inside its own cna_configure_vendored_sdl() FUNCTION, and
        # IMPORTED targets are directory-scoped, so SDL3::SDL3 simply does not
        # exist in this project's scope -- referencing it (as ../cna-samples
        # does, via SDL3::SDL3-static) fails at generate time with "target not
        # found". Linking CNA is enough: its own link interface carries the
        # static SDL archives through.
        target_link_libraries(${target_name} PRIVATE CNA SHARP_RUNTIME)
        set_target_properties(${target_name} PROPERTIES SUFFIX ".html")
        target_link_options(${target_name} PRIVATE
            -sALLOW_MEMORY_GROWTH=1
            -sFORCE_FILESYSTEM=1
            "-sMIN_WEBGL_VERSION=2"
            "-sMAX_WEBGL_VERSION=2"
        )
    else()
        target_link_libraries(${target_name} PRIVATE CNA SHARP_RUNTIME)
    endif()

    if(TARGET SDL3::SDL3main)
        target_link_libraries(${target_name} PRIVATE SDL3::SDL3main)
    endif()

    # CNA_Net (NetworkSession, PacketWriter/Reader, ...) pulls in CNA_GamerServices
    # (GamerServicesComponent, Guide, SignedInGamer, ...) transitively via its own
    # PUBLIC link -- lives in a separate static-library target from the umbrella
    # CNA target, so it needs its own explicit link (see ../cna-samples' identical fix).
    if(TARGET CNA_Net)
        target_link_libraries(${target_name} PRIVATE CNA_Net)
    endif()

    if(WIN32)
        set_target_properties(${target_name} PROPERTIES WIN32_EXECUTABLE TRUE)
        if(COMMAND cna_copy_sdl_runtime)
            cna_copy_sdl_runtime(${target_name})
        endif()
    endif()

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Content"
            "$<TARGET_FILE_DIR:${target_name}>/Content"
        VERBATIM
    )

    # ---------------------------------------------------------------------
    # Borrowed content: copied from ../cna at build time, never committed here.
    #
    # The Content area's .xnb demos need real, externally-produced .xnb files,
    # and CNA consumes that format without ever writing it -- so they cannot be
    # generated locally the way every other asset in this repo is. The only
    # available fixtures are MonoGame-produced ones under ../cna/tests/assets,
    # which are Ms-PL. This repo is MIT and states that it ships no ported
    # Microsoft content, so they are copied into the build output instead of
    # being vendored into version control. (One of them, FontCalibri14.xnb,
    # also embeds a rasterised Calibri glyph atlas -- a proprietary typeface --
    # and is excluded entirely, below.)
    #
    # ../cna is already a hard dependency of this build (add_subdirectory), so
    # this introduces nothing new. It is still guarded: a missing directory
    # leaves the demos to report the absence on screen rather than fail to build.
    # ---------------------------------------------------------------------
    # ---------------------------------------------------------------------
    # Borrowed source: XactFileGen.hpp, included from ../cna, never copied.
    #
    # The Audio area's XACT demos need real .xgs/.xsb/.xwb bank files, and CNA
    # reads that format without ever writing it -- so, like the .xnb fixtures
    # below, they cannot be produced locally. ../cna already solved this for its
    # own demo_xact program with a 389-line, dependency-free generator.
    #
    # That header is Ms-PL (it lives in the Ms-PL cna repository) and this repo
    # is MIT, so it is added to the include path rather than copied into version
    # control. Compiling against it is no different from linking CNA itself,
    # which is equally Ms-PL; what matters is that no Ms-PL source enters this
    # repository's history.
    #
    # Guarded: without it the XACT demos compile to a screen explaining why they
    # are unavailable, instead of failing the build.
    # ---------------------------------------------------------------------
    set(_cna_xact_filegen "${CMAKE_CURRENT_SOURCE_DIR}/../cna/examples/demo_xact/src")
    if(EXISTS "${_cna_xact_filegen}/XactFileGen.hpp")
        target_include_directories(${target_name} PRIVATE "${_cna_xact_filegen}")
        target_compile_definitions(${target_name} PRIVATE CNA_EXAMPLES_HAS_XACT_FILEGEN=1)
        message(STATUS "cna-examples: XACT bank generator found at ${_cna_xact_filegen}")
    else()
        message(STATUS "cna-examples: no XactFileGen.hpp at ${_cna_xact_filegen}"
                       " -- the Audio area's XACT demos will report it as unavailable")
    endif()

    set(_cna_xnb_fixtures "${CMAKE_CURRENT_SOURCE_DIR}/../cna/tests/assets/xnb")
    if(EXISTS "${_cna_xnb_fixtures}")
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${_cna_xnb_fixtures}"
                "$<TARGET_FILE_DIR:${target_name}>/Content/ContentDemo/xnb"
            # Excluded on licensing grounds, not technical ones -- see above.
            # The .decompressed.bin goes too: it is the same Calibri glyph atlas
            # with the LZX container removed, so keeping it would defeat the point.
            COMMAND ${CMAKE_COMMAND} -E rm -f
                "$<TARGET_FILE_DIR:${target_name}>/Content/ContentDemo/xnb/monogame/windows/lzx/FontCalibri14.xnb"
                "$<TARGET_FILE_DIR:${target_name}>/Content/ContentDemo/xnb/monogame/windows/lzx/FontCalibri14.xnb.manifest.json"
                "$<TARGET_FILE_DIR:${target_name}>/Content/ContentDemo/xnb/monogame/windows/lzx/reference-decompressed/FontCalibri14.decompressed.bin"
            VERBATIM
        )
        message(STATUS "cna-examples: .xnb fixtures will be copied from ${_cna_xnb_fixtures}")
    else()
        message(STATUS "cna-examples: no .xnb fixtures at ${_cna_xnb_fixtures}"
                       " -- the Content area's XNB demos will report them as unavailable")
    endif()
endfunction()
