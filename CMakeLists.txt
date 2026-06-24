cmake_minimum_required(VERSION 3.10)
project(Vibe)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

  file(GLOB IMGUI_SOURCES
        cimgui/cimgui.cpp
        cimgui/cimgui_impl.cpp
        cimgui/imgui/imgui.cpp
        cimgui/imgui/imgui_draw.cpp
        cimgui/imgui/imgui_demo.cpp
        cimgui/imgui/imgui_widgets.cpp
        cimgui/imgui/imgui_tables.cpp
        cimgui/imgui/backends/imgui_impl_opengl3.cpp
        cimgui/imgui/backends/imgui_impl_glfw.cpp
    )

  set(IMGUI_STATIC "no" CACHE STRING "Build as a static library")
  set(IMGUI_FREETYPE "no" CACHE STRING "Build with freetype library")
  set(IMGUI_LIBRARIES glfw)

  #add library and link
  if (IMGUI_STATIC)
    add_library(cimgui STATIC ${IMGUI_SOURCES})
  else (IMGUI_STATIC)
    add_library(cimgui SHARED ${IMGUI_SOURCES})
  endif (IMGUI_STATIC)

  include_directories(cimgui/imgui/examples/libs/gl3w cimgui cimgui/imgui)
  if(WIN32)
    list(APPEND IMGUI_LIBRARIES opengl32)
  else(WIN32) # Unix
    list(APPEND IMGUI_LIBRARIES GL)
  endif(WIN32)

  add_definitions("-DIMGUI_USER_CONFIG=\"cimgui/cimconfig.h\"")
  target_compile_definitions(cimgui PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS=1)
  if (WIN32)
    target_compile_definitions(cimgui PUBLIC IMGUI_IMPL_API=extern\t\"C\"\t__declspec\(dllexport\))
    list(APPEND IMGUI_LIBRARIES imm32)
  else (WIN32)
    target_compile_definitions(cimgui PUBLIC IMGUI_IMPL_API=extern\t\"C\"\t)
  endif (WIN32)

  target_include_directories(cimgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  target_include_directories(cimgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
  set_target_properties(cimgui PROPERTIES PREFIX "")
  target_link_libraries(cimgui ${IMGUI_LIBRARIES})

  set(os_specific_includes
        ${OPENGL_INCLUDE_DIRS}
        openal-soft/include
        lib/glad/include
        glfw/include
        cimgui
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

set(desktop_library_sources
    miniz/miniz.c
    miniz/miniz_zip.c
    miniz/miniz_tdef.c
    miniz/miniz_tinfl.c
    lib/glad/src/glad.c
)

set(project_sources
    src/engine/text_renderer.c
    src/engine/time_util.c
    src/engine/file_util.c
    src/engine/callback.c
    src/engine/strutil.c
    src/engine/texture.c
    src/engine/tilemap.c
    src/engine/sprite.c
    src/engine/easing.c
    src/engine/audio.c
    src/engine/input.c
    src/engine/mesh.c
    src/engine/gfx.c

    src/game/replay_management_screen.c
    src/game/maniahit_display.c
    src/game/results_screen.c
    src/game/settings_menu.c
    src/game/song_select.c
    src/game/main_menu.c
    src/game/song_list.c
    src/game/beatmap.c
    src/game/options.c
    src/game/scoring.c
    src/game/gaming.c
    src/game/replay.c

    src/logging.c
    src/main.c
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
  set(CMAKE_C_FLAGS_DEBUG "-g -ggdb -O0")
  set(CMAKE_C_FLAGS_RELEASE "-g -O2")

  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address)
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

  add_compile_definitions(NOMINMAX)
  add_executable(${PROJECT_NAME} ${project_sources} ${library_sources} ${desktop_library_sources})
  add_dependencies(${PROJECT_NAME} copy_assets copy_skin)

  if (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${OPENGL_LIBRARIES}
            alsoft.common
            cimgui
            OpenAL
            glfw
            m
        )
  else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${OPENGL_LIBRARIES}
            alsoft.common
            cimgui
            OpenAL
            glfw
        )
  endif()

endif()
