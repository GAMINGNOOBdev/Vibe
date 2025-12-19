cmake_minimum_required(VERSION 3.10)
project(Vibe)

string(TIMESTAMP NOW "%Y %m %d")
string(REPLACE " " ";" DATE_LIST ${NOW})
list(GET DATE_LIST 0 YEAR)
list(GET DATE_LIST 1 MONTH)
list(GET DATE_LIST 2 DAY)

math(EXPR YEAR ${YEAR})
math(EXPR MONTH ${MONTH})
math(EXPR DAY ${DAY})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

add_compile_definitions(__YEAR__=${YEAR})
add_compile_definitions(__MONTH__=${MONTH})
add_compile_definitions(__DAY__=${DAY})

if(NOT PSP)
    # glfw options
    set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only")
    set( GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only" )
    set( GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" )
    set( GLFW_INSTALL OFF CACHE BOOL "GLFW lib only" )
    set( GLFW_LIBRARY_TYPE "SHARED" )
    set( BUILD_SHARED_LIBS ON )
    # OpenAL Options
    set(ALSOFT_UTILS off)
    set(ALSOFT_RTKIT off)
    set(ALSOFT_INSTALL off)
    set(ALSOFT_EXAMPLES off)
    set(ALSOFT_INSTALL_UTILS off)
    set(ALSOFT_INSTALL_CONFIG off)
    set(ALSOFT_INSTALL_EXAMPLES off)
    set(ALSOFT_INSTALL_HRTF_DATA off)
    set(ALSOFT_INSTALL_AMBDEC_PRESETS off)

    find_package(OpenGL REQUIRED)

    if(NOT UNIX)
        link_directories(glew-windows-2.1.0/lib/Release/x64)
        set(os_specific_glew_include glew-windows-2.1.0/include)
    else()
        set(os_specific_glew_include)
    endif()

    set(os_specific_includes
        ${os_specific_glew_include}
        ${OPENGL_INCLUDE_DIRS}
        openal-soft/include
        glfw/include
    )

    add_subdirectory(openal-soft)
    add_subdirectory(glfw)

else()
    set(os_specific_includes)
endif()

include_directories(
    inc
    lib
    gu2gl
    cglm/include
    miniz
    minimp3
    libwave/include
    ${os_specific_includes}
)

set(library_sources
    libwave/src/wave.c
    lib/stb_vorbis.c
)

set(linux_library_sources
    miniz/miniz.c
    miniz/miniz_zip.c
    miniz/miniz_tdef.c
    miniz/miniz_tinfl.c
)

set(project_sources
    src/audio.c
    src/beatmap.c
    src/easing.c
    src/gaming.c
    src/input.c
    src/main.c
    src/mesh.c
    src/replay_management_screen.c
    src/replay.c
    src/results_screen.c
    src/settings_menu.c
    src/song_select.c
    src/strutil.c
    src/texture.c
    src/time_util.c
    src/callback.c
    src/file_util.c
    src/gfx.c
    src/logging.c
    src/maniahit_display.c
    src/main_menu.c
    src/options.c
    src/scoring.c
    src/song_list.c
    src/sprite.c
    src/text_renderer.c
    src/tilemap.c
)

add_compile_definitions(
    NOMINMAX
    MINIZ_NO_TIME
    AL_LIBTYPE_STATIC
    MINIMP3_NO_SIMD
    __GLIBC_USE\(...\)=0
)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Assets ${CMAKE_CURRENT_BINARY_DIR}/bin/Assets
)
add_custom_target(copy_skin
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Skin ${CMAKE_CURRENT_BINARY_DIR}/bin/Skin
)

set(C_STANDARD "99")

if (UNIX)
    set(CMAKE_C_FLAGS_DEBUG "-g -ggdb")
    set(CMAKE_C_FLAGS_RELEASE "-g")
endif()

if(PSP)
    add_executable(${PROJECT_NAME} ${project_sources} ${library_sources})
    add_dependencies(${PROJECT_NAME} copy_assets copy_skin)

    add_compile_definitions(__PSP__)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        pspaudiocodec
        pspaudiolib
        pspgum_vfpu
        pspdisplay
        intrafont
        psppower
        pspaudio
        pspctrl
        pspvfpu
        psphttp
        pspsdk
        psprtc
        pspgu
        pspge
        m
    )

    create_pbp_file(TARGET ${PROJECT_NAME} TITLE "${CMAKE_PROJECT_NAME}"
        ICON_PATH ${CMAKE_CURRENT_LIST_DIR}/Assets/LOGO.png
    )

else()

    add_compile_definitions(NOMINMAX GLEW_STATIC)
    add_executable(${PROJECT_NAME} ${project_sources} ${library_sources} ${linux_library_sources})
    add_dependencies(${PROJECT_NAME} copy_assets copy_skin)

    if (UNIX)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            ${OPENGL_LIBRARIES}
            GLEW
            alsoft.common
            OpenAL
            glfw
            m
        )
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE
            ${OPENGL_LIBRARIES}
            glew32s
            alsoft.common
            OpenAL
            glfw
        )
    endif()

endif()
