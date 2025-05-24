#include <text_renderer.h>
#include <song_select.h>
#include <options.h>
#include <texture.h>
#include <logging.h>
#include <beatmap.h>
#include <easing.h>
#include <gaming.h>
#include <sprite.h>
#include <memory.h>
#include <audio.h>
#include <input.h>
#include <time.h>
#ifdef __PSP__
#include <gu2gl.h>
#else
#include <GL/gl.h>
#include <pctypes.h>
#include <cglm/cglm.h>
#endif

#include <gfx.h>
#include <app.h>

#define MAX_OBJECTS_ON_SCREEN 256

////////////////
///          ///
///   DATA   ///
///          ///
////////////////

beatmap_t gaming_beatmap = {0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL};
audio_stream_t gaming_audio_stream = {0};
uint8_t gaming_show_results_screen = 0;
int gaming_time = 0;

size_t gaming_beatmap_drawlist_index = MAX_OBJECTS_ON_SCREEN;
beatmap_hitobject_t gaming_drawlist[MAX_OBJECTS_ON_SCREEN];

texture_t gaming_long_note_texture;
texture_t gaming_note1_texture;
texture_t gaming_note2_texture;
sprite_t gaming_long_note;
sprite_t gaming_note;

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

    LOGDEBUG("beatmap folder:");
    LOGDEBUG(beatmap_folder);
    LOGDEBUG("beatmap path:");
    LOGDEBUG(beatmap_path);
    LOGDEBUG("audio sub-path/name:");
    LOGDEBUG(stringf("0x%16.16x", gaming_beatmap.audio_path));
    LOGDEBUG(gaming_beatmap.audio_path);
    audio_stream_load(&gaming_audio_stream, stringf("%s/%s", beatmap_folder, gaming_beatmap.audio_path));
    audio_stream_seek_start(&gaming_audio_stream);
    audio_set_stream(&gaming_audio_stream);

    gaming_audio_stream.volume = options.music_volume;

    gaming_time = 0;
    gaming_beatmap_drawlist_index = MAX_OBJECTS_ON_SCREEN;
    size_t gaming_drawlist_size = MAX_OBJECTS_ON_SCREEN;
    if (gaming_beatmap.object_count < gaming_drawlist_size)
        gaming_drawlist_size = gaming_beatmap.object_count;

    for (int i = 0; i < MAX_OBJECTS_ON_SCREEN; i++)
        gaming_drawlist[i].time = -1;

    memcpy(gaming_drawlist, gaming_beatmap.objects, MAX_OBJECTS_ON_SCREEN * sizeof(beatmap_hitobject_t));

    LOGINFO("loaded REAL gaming");
}

uint8_t gaming_initialized = 0;
void gaming_init(void)
{
    if (gaming_initialized)
        return;

    texture_load(&gaming_long_note_texture, "Skin/LNTail.png", GL_TRUE, GL_TRUE);
    texture_load(&gaming_note1_texture, "Skin/mania-note1.png", GL_TRUE, GL_TRUE);
    texture_load(&gaming_note2_texture, "Skin/mania-note2.png", GL_TRUE, GL_TRUE);
    sprite_create(&gaming_long_note, 0, 0, 30, 512, &gaming_long_note_texture);
    sprite_create(&gaming_note, 0, 0, 30, 12.5f, &gaming_note1_texture);

    gaming_initialized = 1;
}

void gaming_dispose(void)
{
    if (!gaming_initialized)
        return;

    texture_dispose(&gaming_long_note_texture);
    texture_dispose(&gaming_note1_texture);
    texture_dispose(&gaming_note2_texture);
    sprite_dispose(&gaming_long_note);
    sprite_dispose(&gaming_note);
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

    #ifdef __PSP__
    glBlendFunc(GL_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 0, 0);
    glEnable(GL_BLEND);

    glClearColor(0xFF111111);
    #else
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0x11/255.f, 0x11/255.f, 0x11/255.f, 0xFF/255.f);
    #endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    #ifdef __PSP__
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, PSP_SCREEN_WIDTH, 0, PSP_SCREEN_HEIGHT, -0.01f, 10.0f);
    #else
    mat4 projection = GLM_MAT4_IDENTITY_INIT;
    glm_ortho(0, PSP_SCREEN_WIDTH, 0, PSP_SCREEN_HEIGHT, -0.01f, 10.0f, projection);
    graphics_projection_matrix(projection);
    #endif

    if (options.flags.show_fps)
        text_renderer_draw(stringf("gaming at %d fps.", time_fps()), 0, 0, 8);
    else
        text_renderer_draw("gaming.", 0, 0, 8);

    if (!gaming_beatmap.is_pure_4k)
        text_renderer_draw("NOT 4K", 192, 0, 8);

    if (gaming_show_results_screen)
        text_renderer_draw("RESULTS", 188, 100, 8);

    float scrollspeedfactor = options.scroll_speed / (float)SCROLL_SPEED_MAX;
    for (size_t i = 0; i < MAX_OBJECTS_ON_SCREEN; i++)
    {
        beatmap_hitobject_t hitobject = gaming_drawlist[i];
        if (hitobject.time == -1)
            continue;

        texture_t* texture = &gaming_note1_texture;
        uint8_t column = hitobject.column;
        if (column == 1 || column == 2)
            texture = &gaming_note2_texture;

        gaming_note.x = 240 + (column-2)*40;
        gaming_note.y = (hitobject.time - gaming_time) * scrollspeedfactor;
        float lnheight = 0;
        gaming_long_note.height = 0;

        if (hitobject.isLN)
        {
            lnheight = (hitobject.end-hitobject.time) * scrollspeedfactor;
            gaming_long_note.height = lnheight;
            gaming_long_note.x = gaming_note.x;
            gaming_long_note.y = gaming_note.y;
        }

        if (gaming_note.y+lnheight < -(gaming_note.height+0.1f))
        {
            if (hitobject.isLN)
                LOGDEBUG(stringf("note y,start,end,time.noteheight,height, %2.2f|%d|%d|%d|%2.2f|%2.2f", gaming_note.y, hitobject.time, hitobject.end, gaming_time, gaming_note.height, gaming_long_note.height));

            if (gaming_beatmap_drawlist_index + 1 >= gaming_beatmap.object_count)
                gaming_drawlist[i].time = -1;
            else
            {
                gaming_beatmap_drawlist_index++;
                gaming_drawlist[i] = gaming_beatmap.objects[gaming_beatmap_drawlist_index];
            }

            continue;
        }

        if (hitobject.isLN)
            sprite_draw(&gaming_long_note, &gaming_long_note_texture);
        sprite_draw(&gaming_note, texture);
    }

    if (!options.flags.show_debug_info)
        return;

    const char* debug_text = stringf("Timing points: %ld\nObjects: %ld\nTime: %d\nLength: %d\nDrawlist index: %d",
                                     gaming_beatmap.timing_point_count, gaming_beatmap.object_count, gaming_time, gaming_audio_stream.length_ms, gaming_beatmap_drawlist_index);
    text_renderer_draw(debug_text, 5, 264, 8);
}
