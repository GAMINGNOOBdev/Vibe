#include "file_util.h"
#ifdef __PSP__
#include <pspdisplay.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#endif

#include <results_screen.h>
#include <text_renderer.h>
#include <song_select.h>
#include <main_menu.h>
#include <time_util.h>
#include <callback.h>
#include <options.h>
#include <logging.h>
#include <gaming.h>
#include <audio.h>
#include <input.h>
#include <gfx.h>
#include <app.h>

#ifdef __PSP__
PSP_MODULE_INFO("VIBE", 0, 0, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
#else
#include <pctypes.h>
#endif

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#define MINIMP3_IMPLEMENTATION
#include <minimp3.h>
#include <minimp3_ex.h>

FILE* logFile;

void empty_update(float _) {}
void empty_render(void) {}

app_update_callback_t update_callback = empty_update;
app_render_callback_t render_callback = empty_render;

void app_set_update_callback(app_update_callback_t update)
{
    update_callback = update;

    if (update_callback == NULL)
        update_callback = empty_update;
}
app_update_callback_t app_get_update_callback(void) { return update_callback; }

void app_set_render_callback(app_render_callback_t render)
{
    render_callback = render;

    if (render_callback == NULL)
        render_callback = empty_render;
}
app_render_callback_t app_get_render_callback(void) { return render_callback; }

#ifdef __PSP__
#elif defined(_WIN32)
#include <windows.h>

DWORD WINAPI audio_thread(LPVOID _)
{
    while (is_running())
    {
        audio_update();
    }
    return 0;
}

HANDLE audio_thread_handle;
DWORD audio_thread_id;
void setup_audio_thread(void)
{
    audio_thread_handle = CreateThread(NULL, 0, audio_thread, NULL, 0, &audio_thread_id);
}
#else
#include <pthread.h>

void* audio_thread(void* _)
{
    while (is_running())
    {
        audio_update();
    }
    return NULL;
}

pthread_t audio_thread_id;
void setup_audio_thread(void)
{
    pthread_create(&audio_thread_id, NULL, audio_thread, NULL);
}
#endif

extern int ExitRequest;
int main(void)
{
    setup_callbacks();
    remove("game.log");
    logFile = fopen("game.log", "wb+");
    log_set_stream(logFile);
    log_enable_debug_messages(1);

    options_load();

    if (!file_util_directory_exists("Replays"))
        file_util_create_directory("Replays");
    if (!file_util_directory_exists("Songs"))
        file_util_create_directory("Songs");

    audio_init();
    if (ExitRequest)
    {
        LOGERROR("Failed to initialize audio.");
        return -1;
    }

    graphics_init();
    if (ExitRequest)
    {
        LOGERROR("Failed to initialize graphics.");
        return -1;
    }
    input_enable(PSP_CTRL_MODE_ANALOG);
    time_init();
    text_renderer_initialize();

    options_apply();

    switch_to_main_menu();
    #ifndef __PSP__
    setup_audio_thread();
    #endif

    while (is_running())
    {
        input_read();

        if (update_callback)
            update_callback(time_delta());

        graphics_start_frame();

        if (render_callback)
            render_callback();

        graphics_end_frame();

        time_tick();
    }

    options_save();

    text_renderer_dispose();
    main_menu_dispose();
    song_select_dispose();
    gaming_dispose();
    results_screen_dispose();

    #ifdef __PSP__
    #elif defined(_WIN32)
    CloseHandle(audio_thread_handle);
    #else
    pthread_join(audio_thread_id, NULL);
    #endif

    audio_dispose();
    graphics_dispose();
    fclose(logFile);
    exit_game();

    return 0;
}
