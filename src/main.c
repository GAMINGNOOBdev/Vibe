#ifdef __PSP__
#include <pspdisplay.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#endif

#include <text_renderer.h>
#include <song_select.h>
#include <main_menu.h>
#include <callback.h>
#include <options.h>
#include <logging.h>
#include <malloc.h>
#include <audio.h>
#include <input.h>
#include <time.h>
#include <gfx.h>
#include <app.h>

#ifdef __PSP__
PSP_MODULE_INFO("VIBE", 0, 0, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
#else
#include <pctypes.h>
#include <GL/glew.h>
#endif

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>

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
app_update_callback_t app_get_update_callback() { return update_callback; }

void app_set_render_callback(app_render_callback_t render)
{
    render_callback = render;

    if (render_callback == NULL)
        render_callback = empty_render;
}
app_render_callback_t app_get_render_callback() { return render_callback; }

int main()
{
    setup_callbacks();
    remove("game.log");
    logFile = fopen("game.log", "wb+");
    log_set_stream(logFile);
    log_enable_debug_messages(1);

    options_load();

    input_enable(PSP_CTRL_MODE_ANALOG);
    audio_init();
    time_init();
    graphics_init();
    text_renderer_initialize();

    options_apply();

    switch_to_main_menu();

    while (is_running())
    {
        #ifndef __PSP__
        glViewport(0, 0, PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT);
        #endif

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

    audio_dispose();
    graphics_dispose();
    fclose(logFile);
    exit_game();

    return 0;
}
