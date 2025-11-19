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

add_compile_definitions(__YEAR__=${YEAR})
add_compile_definitions(__MONTH__=${MONTH})
add_compile_definitions(__DAY__=${DAY})

include_directories(
    inc
    lib
    gu2gl
    cglm/include
    miniz
    minimp3
    libwave/include
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
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Assets ${CMAKE_CURRENT_BINARY_DIR}/Assets
)
add_custom_target(copy_skin
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Skin ${CMAKE_CURRENT_BINARY_DIR}/Skin
)

set(C_STANDARD "99")

set(CMAKE_C_FLAGS_DEBUG "-g -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-g")

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
        Assets/LOGO.png
        # BACKGROUND_PATH
        # PREVIEW_PATH
    )

else()

    add_executable(${PROJECT_NAME} ${project_sources} ${library_sources} ${linux_library_sources})
    add_dependencies(${PROJECT_NAME} copy_assets copy_skin)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        openal
        GLEW
        glfw
        GL
        m
    )

endif()
