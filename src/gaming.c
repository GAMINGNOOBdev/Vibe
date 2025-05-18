#include <text_renderer.h>
#include <song_select.h>
#include <options.h>
#include <texture.h>
#include <logging.h>
#include <beatmap.h>
#include <easing.h>
#include <gaming.h>
#include <sprite.h>
#include <audio.h>
#include <input.h>
#include <gu2gl.h>
#include <pspgu.h>
#include <gfx.h>
#include <app.h>

////////////////
///          ///
///   DATA   ///
///          ///
////////////////

beatmap_t gaming_beatmap = {0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL};
audio_stream_t gaming_audio_stream = {0};
uint8_t gaming_show_results_screen = 0;
uint8_t gaming_show_beatmap_info = 0;
int gaming_time = 0;

/////////////////////
///               ///
///   FUNCTIONS   ///
///               ///
/////////////////////

void gaming_audio_end_callback(void)
{
    audio_set_stream(NULL);
    gaming_show_results_screen = 1;
}

void switch_to_gaming(const char* beatmap_folder, const char* beatmap_path)
{
    gaming_init();

    gaming_show_results_screen = 0;

    app_set_update_callback(gaming_update);
    app_set_render_callback(gaming_render);

    audio_set_end_callback(gaming_audio_end_callback);
    
    easing_disable();
    
    beatmap_parse(&gaming_beatmap, beatmap_path);

    audio_stream_load(&gaming_audio_stream, stringf("%s/%s", beatmap_folder, gaming_beatmap.audio_path));
    audio_stream_seek_start(&gaming_audio_stream);
    audio_set_stream(&gaming_audio_stream);

    LOGINFO("loaded REAL gaming");
}

uint8_t gaming_initialized = 0;
void gaming_init(void)
{
    if (gaming_initialized)
        return;

    gaming_initialized = 1;
}

void gaming_dispose(void)
{
    if (!gaming_initialized)
        return;

    /// asdfghjkl...
}

void gaming_update(float delta)
{
    gaming_time = audio_stream_get_position(&gaming_audio_stream);

    int confirm = button_pressed_once(PSP_CTRL_CROSS);
    int back = button_pressed_once(PSP_CTRL_CIRCLE);

    if (back)
    {
        switch_to_song_select();
        audio_set_stream(NULL);
        audio_stream_dispose(&gaming_audio_stream);
        beatmap_dispose(&gaming_beatmap);
    }

    if (confirm)
        gaming_show_beatmap_info = !gaming_show_beatmap_info;

    if (button_pressed(PSP_CTRL_UP))
    {
        audio_set_volume(audio_get_volume()+delta);
        options.master_volume = audio_get_volume();
    }

    if (button_pressed(PSP_CTRL_DOWN))
    {
        audio_set_volume(audio_get_volume()-delta);
        options.master_volume = audio_get_volume();
    }

    if (button_pressed_once(PSP_CTRL_START))
        gaming_audio_end_callback();
}

void gaming_render(void)
{
    glDisable(GL_DEPTH_TEST);

    glBlendFunc(GL_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 0, 0);
    glEnable(GL_BLEND);

    glClearColor(0xFF333333);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    text_renderer_draw("gaming.", 0, 0, 8);

    if (!gaming_beatmap.is_pure_4k)
        text_renderer_draw("NOT 4K", 192, 0, 8);

    if (gaming_show_results_screen)
        text_renderer_draw("RESULTS", 188, 100, 8);

    if (!gaming_show_beatmap_info)
        return;

    text_renderer_draw(stringf("Audio: '%s'", gaming_beatmap.audio_path), 5, 264, 8);
    text_renderer_draw(stringf("AR: %2.2f", gaming_beatmap.ar), 5, 256, 8);
    text_renderer_draw(stringf("OD: %2.2f", gaming_beatmap.od), 5, 248, 8);
    text_renderer_draw(stringf("HP: %2.2f", gaming_beatmap.hp), 5, 240, 8);
    text_renderer_draw(stringf("SM: %2.2f", gaming_beatmap.slider_multiplier), 5, 232, 8);
    text_renderer_draw(stringf("ST: %2.2f", gaming_beatmap.slider_tickrate), 5, 224, 8);
    text_renderer_draw(stringf("Timing points: %ld", gaming_beatmap.timing_point_count), 5, 216, 8);
    text_renderer_draw(stringf("Objects: %ld", gaming_beatmap.object_count), 5, 208, 8);
    text_renderer_draw(stringf("Time: %d", gaming_time), 5, 200, 8);
    int length = gaming_audio_stream.length * 1000.f;
    text_renderer_draw(stringf("Length: %d", length), 5, 192, 8);
}
