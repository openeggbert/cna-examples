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
        target_link_libraries(${target_name} PRIVATE
            CNA SDL3::SDL3-static SHARP_RUNTIME)
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
endfunction()
