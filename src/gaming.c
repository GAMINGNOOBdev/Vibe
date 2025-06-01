#include <text_renderer.h>
#include <song_select.h>
#include <options.h>
#include <texture.h>
#include <logging.h>
#include <beatmap.h>
#include <scoring.h>
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

#define MAX_OBJECTS_ON_SCREEN 128

////////////////
///          ///
///   DATA   ///
///          ///
////////////////

beatmap_t gaming_beatmap = {0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL};
audio_stream_t gaming_audio_stream = {0};
uint8_t gaming_show_results_screen = 0;
float gaming_time = 0;

size_t gaming_beatmap_drawlist_index = MAX_OBJECTS_ON_SCREEN;
beatmap_hitobject_t gaming_drawlist[MAX_OBJECTS_ON_SCREEN];
beatmap_timing_point_t gaming_current_timing_point;
size_t gaming_timing_point_index;
float gaming_scroll_speed_base;
float gaming_beat_length;

texture_t gaming_long_note_texture;
texture_t gaming_note1_texture;
texture_t gaming_note2_texture;
texture_t gaming_judgementline_texture;
sprite_t gaming_long_note;
sprite_t gaming_note;

score_t score;

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

    gaming_beatmap_drawlist_index = MAX_OBJECTS_ON_SCREEN;
    size_t gaming_drawlist_size = MAX_OBJECTS_ON_SCREEN;
    if (gaming_beatmap.object_count < gaming_drawlist_size)
        gaming_drawlist_size = gaming_beatmap.object_count;

    for (int i = 0; i < MAX_OBJECTS_ON_SCREEN; i++)
        gaming_drawlist[i].time = -1;

    memcpy(gaming_drawlist, gaming_beatmap.objects, MAX_OBJECTS_ON_SCREEN * sizeof(beatmap_hitobject_t));

    gaming_scroll_speed_base = 11485.f / (float)options.scroll_speed;
    gaming_beat_length = 1;

    gaming_timing_point_index = 0;
    gaming_current_timing_point = gaming_beatmap.timing_points[gaming_timing_point_index];
    gaming_timing_point_index++;
    if (gaming_current_timing_point.uninherited)
        gaming_beat_length = gaming_current_timing_point.beatLength;

    score_calculator_init(&score);
    score_calculator_clear();
    score_calculator_set_difficulty(gaming_beatmap.od);

    gaming_time = audio_stream_get_position(&gaming_audio_stream);

    LOGINFO("loaded REAL gaming");
}

uint8_t gaming_initialized = 0;
void gaming_init(void)
{
    if (gaming_initialized)
        return;

    texture_load(&gaming_judgementline_texture, "Skin/mania-stage-hint.png", GL_TRUE, GL_TRUE);
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

    texture_dispose(&gaming_judgementline_texture);
    texture_dispose(&gaming_long_note_texture);
    texture_dispose(&gaming_note1_texture);
    texture_dispose(&gaming_note2_texture);
    sprite_dispose(&gaming_long_note);
    sprite_dispose(&gaming_note);
}

void gaming_update(float delta)
{
    gaming_time = audio_stream_get_position(&gaming_audio_stream);
    if (gaming_current_timing_point.time < gaming_time && gaming_timing_point_index < gaming_beatmap.timing_point_count)
    {
        gaming_current_timing_point = gaming_beatmap.timing_points[gaming_timing_point_index];
        gaming_timing_point_index++;

        if (gaming_current_timing_point.uninherited)
            gaming_beat_length = gaming_current_timing_point.beatLength;
    }

    if (button_pressed_once(PSP_CTRL_START))
    {
        switch_to_song_select();
        audio_set_stream(NULL);
        audio_stream_dispose(&gaming_audio_stream);
        beatmap_dispose(&gaming_beatmap);
    }

    if (button_pressed_once(PSP_CTRL_SELECT))
        gaming_audio_end_callback();
}

void gaming_handle_note_inputs()
{
    uint8_t columns_hit_once[] = {
        0, 0, 0, 0
    };
    uint8_t columns_held[] = {
        0, 0, 0, 0
    };
    uint8_t column_occupied[] = {
        0, 0, 0, 0
    };

    for (int i = 0; i < 4; i++)
    {
        int key = options.keybinds.m4l1;
        if (i == 1)
            key = options.keybinds.m4l2;
        else if (i == 2)
            key = options.keybinds.m4l3;
        else if (i == 3)
            key = options.keybinds.m4l4;

        columns_hit_once[i] = button_pressed_once(key);
        columns_held[i] = button_pressed(key);
    }

    for (size_t i = 0; i < MAX_OBJECTS_ON_SCREEN; i++)
    {
        beatmap_hitobject_t hitobject = gaming_drawlist[i];
        if (hitobject.time == -1 || hitobject.time - gaming_time > 1000.f)
            continue;
        if (!score_calculator_should_be_considered(hitobject.time, gaming_time))
            continue;

        uint8_t column = hitobject.column;
        if (column_occupied[column])
            continue;
        column_occupied[column] = 1;

        if (!hitobject.isLN)
        {
            if (hitobject.hit)
                continue;

            if (columns_hit_once[column])
                gaming_drawlist[i].hit = score_calculator_judge(&hitobject, gaming_time);
            continue;
        }

        if (!hitobject.held && columns_hit_once[column])
        {
            if (score_calculator_judge(&hitobject, gaming_time))
            {
                hitobject.hit = gaming_drawlist[i].hit = 1;
                hitobject.held = gaming_drawlist[i].held = 1;
            }
        }

        if (hitobject.held && !hitobject.tailHit)
        {
            if (!columns_held[column])
            {
                if (score_calculator_should_be_considered(hitobject.end, gaming_time))
                {
                    if (score_calculator_judge_release(&hitobject, gaming_time))
                        hitobject.tailHit = gaming_drawlist[i].tailHit = 1;
                }
                else
                {
                    score_calculator_judge_as(JudgementMiss);
                    hitobject.tailHit = gaming_drawlist[i].tailHit = 1;
                }
            }
        }
    }
}

void gaming_render(void)
{
    gaming_handle_note_inputs();

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
        text_renderer_draw(stringf("%d score gaming at %d fps.", score.total_score, time_fps()), 0, 0, 8);
    else
        text_renderer_draw(stringf("%d score gaming.", score.total_score), 0, 0, 8);

    if (!gaming_beatmap.is_pure_4k)
        text_renderer_draw("NOT 4K", 192, 0, 8);

    if (gaming_show_results_screen)
        text_renderer_draw("RESULTS", 188, 100, 8);

    float sv = 1.0f;

    for (size_t i = 0; i < MAX_OBJECTS_ON_SCREEN; i++)
    {
        beatmap_hitobject_t hitobject = gaming_drawlist[i];
        if (hitobject.time == -1)
            continue;

        if (hitobject.time - gaming_time > 1000.f)
            continue;

        texture_t* texture = &gaming_note1_texture;
        uint8_t column = hitobject.column;
        gaming_note.x = 240 + (column-2)*35;
        if (column == 1 || column == 2)
            texture = &gaming_note2_texture;

        sv = beatmap_calculate_sv(gaming_beatmap.timing_points, gaming_beatmap.timing_point_count, hitobject);

        float progress = (hitobject.time - gaming_time);
        float pixels_per_ms = (float)PSP_SCREEN_HEIGHT / gaming_scroll_speed_base;
        gaming_note.y = 20 + progress * pixels_per_ms * sv; /// judgement line at y=20
        float lnheight = 0;
        gaming_long_note.height = 0;

        if (gaming_note.y > PSP_SCREEN_HEIGHT+100.f)
            continue;

        if (hitobject.isLN)
        {
            lnheight = (hitobject.end-hitobject.time) * pixels_per_ms * sv;
            gaming_long_note.height = lnheight;
            gaming_long_note.x = gaming_note.x;
            gaming_long_note.y = gaming_note.y;
        }

        if (gaming_note.y+lnheight < -(gaming_note.height+0.1f) || (hitobject.isLN ? hitobject.hit && hitobject.tailHit : hitobject.hit) || score_calculator_is_missed(hitobject, gaming_time))
        {
            if (!hitobject.hit)
                score_calculator_judge_as(JudgementMiss);

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

    gaming_note.x = 165;
    gaming_note.y = 20;
    float old_height = gaming_note.height;
    gaming_note.height = 6;
    gaming_note.width = 145;
    sprite_draw(&gaming_note, &gaming_judgementline_texture);
    gaming_note.height = old_height;
    gaming_note.width = 30;

    if (!options.flags.show_debug_info)
        return;

    const char* debug_text = stringf("Timing points: %ld\nObjects: %ld\nTime: %2.2f\nDrawlist index: %d\nTiming point (%d|%2.2f|sv: %2.2f)\nNum of: X/meh/ok/gud/grt/pfct/combo\n%d|%d|%d|%d|%d|%d|%d",
                                     gaming_beatmap.timing_point_count, gaming_beatmap.object_count, gaming_time, gaming_beatmap_drawlist_index,
                                     gaming_current_timing_point.uninherited,
                                     gaming_current_timing_point.uninherited ? 1.f / gaming_current_timing_point.beatLength * 1000.f * 60.f : -1, sv,
                                     score.numMiss, score.numMeh, score.numOk, score.numGood, score.numGreat, score.numPerfect, score.combo);
    text_renderer_draw(debug_text, 5, 264, 8);
}
