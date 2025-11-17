#include <results_screen.h>
#include <text_renderer.h>
#include <song_select.h>
#include <time_util.h>
#include <options.h>
#include <texture.h>
#include <logging.h>
#include <beatmap.h>
#include <strutil.h>
#include <scoring.h>
#include <easing.h>
#include <gaming.h>
#include <sprite.h>
#include <memory.h>
#include <audio.h>
#include <input.h>
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

beatmap_t gaming_beatmap = {0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL};
uint8_t gaming_reached_end_of_beatmap = 0;
audio_stream_t gaming_audio_stream = {0};
uint8_t gaming_show_results_screen = 0;
float gaming_time = 0;

gaming_soundinfo_t gaming_soundinfo = {
    .drum_hitclap={0}, .drum_hitnormal={0}, .drum_hitfinish={0}, .drum_hitwhistle={0},
    .soft_hitclap={0}, .soft_hitnormal={0}, .soft_hitfinish={0}, .soft_hitwhistle={0},
    .normal_hitclap={0}, .normal_hitnormal={0}, .normal_hitfinish={0}, .normal_hitwhistle={0},
};

size_t gaming_beatmap_drawlist_index = MAX_OBJECTS_ON_SCREEN;
beatmap_hitobject_t gaming_drawlist[MAX_OBJECTS_ON_SCREEN];
beatmap_timing_point_t gaming_current_timing_point;
beatmap_hitobject_t gaming_last_hitobject;
size_t gaming_timing_point_index;
float gaming_scroll_speed_current;
float gaming_scroll_speed_base;
float gaming_beat_length;

gaming_drawinfo_t gaming_drawinfo;
texture_t* gaming_maniahit_texture = NULL;

score_t gaming_score;
const float GAMING_MAX_TIME_FOR_JUDGEMENT_VISIBLE = 0.25f;
float gaming_judgement_visible_timer = 0;

/////////////////////
///               ///
///   FUNCTIONS   ///
///               ///
/////////////////////

void gaming_audio_end_callback(void)
{
    audio_set_music_stream(NULL);
    gaming_show_results_screen = 1;
}

void gaming_score_judgement_display_callback(scoring_judgement_type_t judgement)
{
    gaming_maniahit_texture = NULL;

    if (judgement == JudgementMiss)
        gaming_maniahit_texture = &gaming_drawinfo.maniahit0_texture;
    else if (judgement == JudgementMeh)
        gaming_maniahit_texture = &gaming_drawinfo.maniahit50_texture;
    else if (judgement == JudgementOk)
        gaming_maniahit_texture = &gaming_drawinfo.maniahit100_texture;
    else if (judgement == JudgementGood)
        gaming_maniahit_texture = &gaming_drawinfo.maniahit200_texture;
    else if (judgement == JudgementGreat)
        gaming_maniahit_texture = &gaming_drawinfo.maniahit300_texture;
    else if (judgement == JudgementPerfect)
        gaming_maniahit_texture = &gaming_drawinfo.maniahit300g_texture;

    if (judgement != JudgementNone)
        gaming_judgement_visible_timer = (float)time_total();
}

void switch_to_gaming(const char* beatmap_folder, const char* beatmap_path)
{
    gaming_init();

    gaming_show_results_screen = 0;

    app_set_update_callback(gaming_update);
    app_set_render_callback(gaming_render);

    easing_disable();

    beatmap_parse(&gaming_beatmap, beatmap_path);

    LOGDEBUG("beatmap folder: '%s'", beatmap_folder);
    LOGDEBUG("beatmap path: '%s'", beatmap_path);
    LOGDEBUG("audio sub-path/name: 0x%16.16x ('%s')", gaming_beatmap.audio_path, gaming_beatmap.audio_path);
    gaming_audio_stream = audio_stream_load(stringf("%s/%s", beatmap_folder, gaming_beatmap.audio_path));
    gaming_audio_stream.end_callback = gaming_audio_end_callback;
    audio_stream_seek_start(&gaming_audio_stream);
    audio_set_music_stream(&gaming_audio_stream);

    gaming_audio_stream.volume = options.music_volume;

    gaming_beatmap_drawlist_index = MAX_OBJECTS_ON_SCREEN;
    size_t gaming_drawlist_size = MAX_OBJECTS_ON_SCREEN;
    if (gaming_beatmap.object_count < gaming_drawlist_size)
        gaming_drawlist_size = gaming_beatmap.object_count;

    for (int i = 0; i < MAX_OBJECTS_ON_SCREEN; i++)
    {
        gaming_drawlist[i] = (beatmap_hitobject_t){
            .time = -1,
            .end = 0,
            .hitsound = 0,
            .column = 0,
            .hit = 0,
            .held = 0,
            .isLN = 0,
            .tailHit = 0,
            .reserved = 0,
        };
    }
    gaming_last_hitobject = (beatmap_hitobject_t){
        .time = -1,
        .end = 0,
        .hitsound = 0,
        .column = 0,
        .hit = 0,
        .held = 0,
        .isLN = 0,
        .tailHit = 0,
        .reserved = 0,
    };

    int numObjects = MAX_OBJECTS_ON_SCREEN;
    if (gaming_beatmap.object_count < MAX_OBJECTS_ON_SCREEN)
        numObjects = gaming_beatmap.object_count;

    memcpy(gaming_drawlist, gaming_beatmap.objects, numObjects * sizeof(beatmap_hitobject_t));
    gaming_last_hitobject = gaming_drawlist[numObjects-1];
    gaming_reached_end_of_beatmap = 0;

    gaming_scroll_speed_base = 11485.f / (float)options.scroll_speed;
    gaming_scroll_speed_current = 1;
    gaming_beat_length = 1;

    gaming_timing_point_index = 0;
    gaming_current_timing_point = gaming_beatmap.timing_points[gaming_timing_point_index];
    gaming_timing_point_index++;
    if (gaming_current_timing_point.uninherited)
        gaming_beat_length = gaming_current_timing_point.beatLength;

    score_calculator_init(&gaming_score);
    score_calculator_clear();
    score_calculator_set_difficulty(gaming_beatmap.od);
    score_calculator_set_total_objects(gaming_beatmap.hit_count);
    score_calculator_set_judgement_callback(gaming_score_judgement_display_callback);

    gaming_time = gaming_audio_stream.seconds;

    LOGINFO("loaded REAL gaming");
}

uint8_t gaming_initialized = 0;
void gaming_init(void)
{
    if (gaming_initialized)
        return;

    texture_load(&gaming_drawinfo.judgementline_texture, "Skin/mania-stage-hint.png", GL_TRUE, GL_TRUE);
    texture_load(&gaming_drawinfo.lanehit_texture, "Skin/mania-lanehit.png", GL_TRUE, GL_TRUE);
    texture_load(&gaming_drawinfo.long_note_texture, "Skin/LNTail.png", GL_TRUE, GL_TRUE);
    texture_load(&gaming_drawinfo.note1_texture, "Skin/mania-note1.png", GL_TRUE, GL_TRUE);
    texture_load(&gaming_drawinfo.note2_texture, "Skin/mania-note2.png", GL_TRUE, GL_TRUE);

    texture_load(&gaming_drawinfo.maniahit300g_texture, "Skin/mania-hit300g.png", GL_TRUE, GL_TRUE);
    texture_load(&gaming_drawinfo.maniahit300_texture, "Skin/mania-hit300.png", GL_TRUE, GL_TRUE);
    texture_load(&gaming_drawinfo.maniahit200_texture, "Skin/mania-hit200.png", GL_TRUE, GL_TRUE);
    texture_load(&gaming_drawinfo.maniahit100_texture, "Skin/mania-hit100.png", GL_TRUE, GL_TRUE);
    texture_load(&gaming_drawinfo.maniahit50_texture, "Skin/mania-hit50.png", GL_TRUE, GL_TRUE);
    texture_load(&gaming_drawinfo.maniahit0_texture, "Skin/mania-hit0.png", GL_TRUE, GL_TRUE);

    sprite_create(&gaming_drawinfo.long_note, 0, 0, 30, 512, &gaming_drawinfo.long_note_texture);
    sprite_create(&gaming_drawinfo.maniahit, (PSP_SCREEN_WIDTH-16)/2.f, PSP_SCREEN_HEIGHT - 48, 16, 16, &gaming_drawinfo.maniahit0_texture);
    sprite_create(&gaming_drawinfo.note, 0, 0, 30, 12.5f, &gaming_drawinfo.note1_texture);

    // gaming_soundinfo.drum_hitclap = audio_stream_load("Skin/drum-hitclap.wav");
    // gaming_soundinfo.drum_hitnormal = audio_stream_load("Skin/drum-hitnormal.wav");
    // gaming_soundinfo.drum_hitfinish = audio_stream_load("Skin/drum-hitfinish.wav");
    // gaming_soundinfo.drum_hitwhistle = audio_stream_load("Skin/drum-hitwhistle.wav");
    // gaming_soundinfo.soft_hitclap = audio_stream_load("Skin/soft-hitclap.wav");
    // gaming_soundinfo.soft_hitnormal = audio_stream_load("Skin/soft-hitnormal.wav");
    // gaming_soundinfo.soft_hitfinish = audio_stream_load("Skin/soft-hitfinish.wav");
    // gaming_soundinfo.soft_hitwhistle = audio_stream_load("Skin/soft-hitwhistle.wav");
    // gaming_soundinfo.normal_hitclap = audio_stream_load("Skin/normal-hitclap.wav");
    // gaming_soundinfo.normal_hitnormal = audio_stream_load("Skin/normal-hitnormal.wav");
    // gaming_soundinfo.normal_hitfinish = audio_stream_load("Skin/normal-hitfinish.wav");
    // gaming_soundinfo.normal_hitwhistle = audio_stream_load("Skin/normal-hitwhistle.wav");

    gaming_initialized = 1;
}

void gaming_dispose(void)
{
    if (!gaming_initialized)
        return;

    gaming_initialized = 0;

    texture_dispose(&gaming_drawinfo.judgementline_texture);
    texture_dispose(&gaming_drawinfo.long_note_texture);
    texture_dispose(&gaming_drawinfo.lanehit_texture);
    texture_dispose(&gaming_drawinfo.note1_texture);
    texture_dispose(&gaming_drawinfo.note2_texture);

    texture_dispose(&gaming_drawinfo.maniahit300g_texture);
    texture_dispose(&gaming_drawinfo.maniahit300_texture);
    texture_dispose(&gaming_drawinfo.maniahit200_texture);
    texture_dispose(&gaming_drawinfo.maniahit100_texture);
    texture_dispose(&gaming_drawinfo.maniahit50_texture);
    texture_dispose(&gaming_drawinfo.maniahit0_texture);

    sprite_dispose(&gaming_drawinfo.long_note);
    sprite_dispose(&gaming_drawinfo.maniahit);
    sprite_dispose(&gaming_drawinfo.note);

    // audio_stream_dispose(&gaming_soundinfo.drum_hitclap);
    // audio_stream_dispose(&gaming_soundinfo.drum_hitnormal);
    // audio_stream_dispose(&gaming_soundinfo.drum_hitfinish);
    // audio_stream_dispose(&gaming_soundinfo.drum_hitwhistle);
    // audio_stream_dispose(&gaming_soundinfo.soft_hitclap);
    // audio_stream_dispose(&gaming_soundinfo.soft_hitnormal);
    // audio_stream_dispose(&gaming_soundinfo.soft_hitfinish);
    // audio_stream_dispose(&gaming_soundinfo.soft_hitwhistle);
    // audio_stream_dispose(&gaming_soundinfo.normal_hitclap);
    // audio_stream_dispose(&gaming_soundinfo.normal_hitnormal);
    // audio_stream_dispose(&gaming_soundinfo.normal_hitfinish);
    // audio_stream_dispose(&gaming_soundinfo.normal_hitwhistle);
}

void gaming_update(float _)
{
    if (gaming_show_results_screen)
    {
        switch_to_results_screen(&gaming_drawinfo, &gaming_score);
        audio_set_music_stream(NULL);
        audio_stream_dispose(&gaming_audio_stream);
        beatmap_dispose(&gaming_beatmap);
    }

    gaming_time = gaming_audio_stream.seconds;
    if (gaming_current_timing_point.time < gaming_time && gaming_timing_point_index < gaming_beatmap.timing_point_count)
    {
        gaming_current_timing_point = gaming_beatmap.timing_points[gaming_timing_point_index];
        gaming_timing_point_index++;

        if (gaming_current_timing_point.uninherited)
            gaming_beat_length = gaming_current_timing_point.beatLength;
        else
            gaming_scroll_speed_current = -100.f / gaming_current_timing_point.beatLength;
    }

    if (gaming_last_hitobject.time != -1)
    {
        float time_diff = gaming_last_hitobject.time - gaming_time;
        if (gaming_last_hitobject.isLN)
            time_diff = gaming_last_hitobject.end - gaming_time;

        if (time_diff < -3000.f && gaming_reached_end_of_beatmap)
            gaming_show_results_screen = 1;
    }

    if ((button_pressed_once(options.keybinds.select) || button_pressed_once(options.keybinds.back)) && gaming_time > 1000.0f)
    {
        if (gaming_audio_stream.playing)
            audio_stream_pause(&gaming_audio_stream);
        else
            audio_stream_resume(&gaming_audio_stream);
    }

    if (button_pressed_once(options.keybinds.start))
    {
        audio_stream_dispose(&gaming_audio_stream);
        beatmap_dispose(&gaming_beatmap);
        audio_set_music_stream(NULL);
        switch_to_song_select();
    }
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
        int key = options.game_keybinds.m4l1;
        if (i == 1)
            key = options.game_keybinds.m4l2;
        else if (i == 2)
            key = options.game_keybinds.m4l3;
        else if (i == 3)
            key = options.game_keybinds.m4l4;

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

        uint8_t hitsound = hitobject.hitsound;
        uint8_t hitnormal = hitsound & 0b1;
        uint8_t hitwhistle = hitsound & 0b10;
        uint8_t hitfinish = hitsound & 0b100;
        uint8_t hitclap = hitsound & 0b1000;

        uint8_t column = hitobject.column;
        if (column_occupied[column])
            continue;
        column_occupied[column] = 1;

        // if (columns_hit_once[column])
        // {
        //     if (hitnormal)
        //         audio_play_sfx_stream(&gaming_soundinfo.normal_hitnormal);
        //     if (hitwhistle)
        //         audio_play_sfx_stream(&gaming_soundinfo.normal_hitwhistle);
        //     if (hitfinish)
        //         audio_play_sfx_stream(&gaming_soundinfo.normal_hitfinish);
        //     if (hitclap)
        //         audio_play_sfx_stream(&gaming_soundinfo.normal_hitclap);
        // }

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

    glClearColor(0xFF000000);
    #else
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0, 0, 0, 0xFF/255.f);
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
        text_renderer_draw(stringf("%d combo gaming at %d fps.", gaming_score.combo, time_fps()), 0, 0, 8);
    else
        text_renderer_draw(stringf("%d combo gaming.", gaming_score.combo), 0, 0, 8);

    if (!gaming_beatmap.is_pure_4k)
        text_renderer_draw("NOT 4K", 192, 0, 8);

    float sv = 1.0f;

    for (size_t i = 0; i < MAX_OBJECTS_ON_SCREEN; i++)
    {
        beatmap_hitobject_t hitobject = gaming_drawlist[i];
        if (hitobject.time == -1)
            continue;

        if (hitobject.time - gaming_time > 2000.f)
            continue;

        texture_t* texture = &gaming_drawinfo.note1_texture;
        uint8_t column = hitobject.column;
        gaming_drawinfo.note.x = 240 + (column-2)*35;
        if (column == 1 || column == 2)
            texture = &gaming_drawinfo.note2_texture;

        sv = beatmap_calculate_sv(gaming_beatmap.timing_points, gaming_beatmap.timing_point_count, hitobject);

        float progress = (hitobject.time - gaming_time);
        float pixels_per_ms = (float)PSP_SCREEN_HEIGHT / gaming_scroll_speed_base;
        gaming_drawinfo.note.y = 20 + progress * pixels_per_ms * sv; /// judgement line at y=20
        float lnheight = 0;
        gaming_drawinfo.long_note.height = 0;

        if (gaming_drawinfo.note.y > PSP_SCREEN_HEIGHT+100.f)
            continue;

        if (hitobject.isLN)
        {
            lnheight = (hitobject.end-hitobject.time) * pixels_per_ms * sv;
            gaming_drawinfo.long_note.height = lnheight;
            gaming_drawinfo.long_note.x = gaming_drawinfo.note.x;
            gaming_drawinfo.long_note.y = gaming_drawinfo.note.y;
        }

        if (gaming_drawinfo.note.y+lnheight < -(gaming_drawinfo.note.height+0.1f) || (hitobject.isLN ? hitobject.hit && hitobject.tailHit : hitobject.hit) || score_calculator_is_missed(hitobject, gaming_time))
        {
            if (!hitobject.hit)
                score_calculator_judge_as(JudgementMiss);

            if (gaming_beatmap_drawlist_index + 1 >= gaming_beatmap.object_count)
            {
                gaming_drawlist[i].time = -1;
                gaming_reached_end_of_beatmap = 1;
            }
            else
            {
                gaming_beatmap_drawlist_index++;
                gaming_last_hitobject = gaming_drawlist[i] = gaming_beatmap.objects[gaming_beatmap_drawlist_index];
            }

            continue;
        }

        if (hitobject.isLN)
            sprite_draw(&gaming_drawinfo.long_note, &gaming_drawinfo.long_note_texture);
        sprite_draw(&gaming_drawinfo.note, texture);
    }

    // draw hit info (300g, 300, 200, 100, 50, MISS)
    if (gaming_maniahit_texture != NULL && (float)time_total() - gaming_judgement_visible_timer < GAMING_MAX_TIME_FOR_JUDGEMENT_VISIBLE)
        sprite_draw(&gaming_drawinfo.maniahit, gaming_maniahit_texture);

    // handle judgement line positioning
    gaming_drawinfo.note.x = 165;
    gaming_drawinfo.note.y = 20;
    float old_height = gaming_drawinfo.note.height;
    gaming_drawinfo.note.height = 6;
    gaming_drawinfo.note.width = 145;

    // draw judgement line
    sprite_draw(&gaming_drawinfo.note, &gaming_drawinfo.judgementline_texture);
    gaming_drawinfo.note.height = 20;
    gaming_drawinfo.note.width = 30;
    for (int i = 0; i < 4; i++)
    {
        int key = options.game_keybinds.m4l1;
        if (i == 1)
            key = options.game_keybinds.m4l2;
        else if (i == 2)
            key = options.game_keybinds.m4l3;
        else if (i == 3)
            key = options.game_keybinds.m4l4;

        if(button_pressed(key))
        {
            gaming_drawinfo.note.x = 240 + (i-2)*35;
            gaming_drawinfo.note.y = 0;
            sprite_draw(&gaming_drawinfo.note, &gaming_drawinfo.lanehit_texture);
        }
    }
    gaming_drawinfo.note.height = old_height;

    if (!options.flags.show_debug_info)
    {
        const char* info_text = stringf("Score: %d\nAcc: %2.2f", gaming_score.total_score, gaming_score.accuracy * 100.f);
        text_renderer_draw(info_text, 5, 264, 8);
        return;
    }

    const char* debug_text = stringf("gaming_judgement_visible_timer: %2.2f", gaming_judgement_visible_timer);
    text_renderer_draw(debug_text, 5, 264, 8);
}
