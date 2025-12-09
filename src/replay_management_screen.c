#include <replay_management_screen.h>
#include <text_renderer.h>
#include <song_select.h>
#include <time_util.h>
#include <texture.h>
#include <options.h>
#include <strutil.h>
#include <logging.h>
#include <sprite.h>
#include <replay.h>
#include <easing.h>
#include <input.h>
#ifdef __PSP__
#   include <gu2gl.h>
#else
#   include <pctypes.h>
#   include <cglm/cglm.h>
#endif
#include <gfx.h>
#include <app.h>

extern sprite_t song_select_background;
extern sprite_t song_select_selector;

extern texture_t song_select_background_texture;
extern texture_t song_select_selector_texture;

#define REPLAYS_ON_SCREEN 6

int replay_selected_index = 0;
int replay_scroll_offset = 0;

replay_manager_return_result_t* replay_manager_data = NULL;
void switch_to_replay_management_screen(uint64_t map_id, uint64_t set_id)
{
    replay_manager_data = replay_manager_search_for_map(set_id, map_id);
    app_set_update_callback(replay_management_screen_update);
    app_set_render_callback(replay_management_screen_render);

    replay_selected_index = 0;
    replay_scroll_offset = 0;

    easing_enable();
    easing_reset_timer();
    easing_set_duration(0.5f);
    easing_set_type(easeOutCubic);
}

void replay_management_screen_update(float _)
{
    int confirm = button_pressed_once(options.keybinds.confirm) || button_pressed_once(options.keybinds.start);
    int back = button_pressed_once(options.keybinds.back);
    int down = button_pressed_once(PSP_CTRL_DOWN);
    int up = button_pressed_once(PSP_CTRL_UP);

    if (back)
        switch_to_song_select();

    if (!replay_manager_data || replay_manager_data->count == 0)
        return;

    if (up && replay_selected_index > 0)
    {
        replay_selected_index--;
        if (replay_selected_index < replay_scroll_offset)
            replay_scroll_offset--;

        easing_reset_timer();
        easing_set_duration(0.5f);
        easing_set_type(easeOutCubic);
    }
    else if (up && replay_selected_index == 0)
    {
        replay_selected_index = replay_manager_data->count-1;
        if (replay_selected_index >= replay_scroll_offset + REPLAYS_ON_SCREEN)
            replay_scroll_offset = replay_selected_index - REPLAYS_ON_SCREEN + 1;

        easing_reset_timer();
        easing_set_duration(0.5f);
        easing_set_type(easeOutCubic);
    }
    if (down && replay_selected_index < (int)replay_manager_data->count - 1)
    {
        replay_selected_index++;
        if (replay_selected_index >= replay_scroll_offset + REPLAYS_ON_SCREEN)
            replay_scroll_offset++;

        easing_reset_timer();
        easing_set_duration(0.5f);
        easing_set_type(easeOutCubic);
    }
    else if (down && replay_selected_index == (int)replay_manager_data->count - 1)
    {
        replay_selected_index = 0;
        replay_scroll_offset = 0;

        easing_reset_timer();
        easing_set_duration(0.5f);
        easing_set_type(easeOutCubic);
    }
    if (confirm && replay_selected_index < (int)replay_manager_data->count)
    {
        easing_reset_timer();
        easing_set_duration(1.5f);
        easing_set_type(easeOutCubic);
        LOGDEBUG("TOOD: --- implement playback ---");
    }
}

void replay_management_screen_render(void)
{
    glDisable(GL_DEPTH_TEST);

    #ifdef __PSP__
    glBlendFunc(GL_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 0, 0);
    glEnable(GL_BLEND);

    glClearColor(0xFF111111);
    #else
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0x33/255.f, 0x33/255.f, 0x33/255.f, 0xFF/255.f);
    #endif

    glClear(GL_COLOR_BUFFER_BIT);

    #ifdef __PSP__
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, PSP_SCREEN_WIDTH, 0, PSP_SCREEN_HEIGHT, -0.01f, 10.0f);
    #else
    mat4 projection = GLM_MAT4_IDENTITY_INIT;
    glm_ortho(0, PSP_SCREEN_WIDTH, 0, PSP_SCREEN_HEIGHT, -0.01f, 10.0f, projection);
    graphics_projection_matrix(projection);
    #endif

    sprite_draw(&song_select_background, &song_select_background_texture);

    if (options.flags.show_fps)
        text_renderer_draw(stringf("%d fps", time_fps()), 0, 0, 8);

    if (!replay_manager_data || replay_manager_data->count == 0)
    {
        text_renderer_draw("No replays for this map", 5, 242, 8);
        return;
    }

    float y_start = 242;
    for (int i = 0; i < REPLAYS_ON_SCREEN; i++)
    {
        int replay_index = replay_scroll_offset + i;

        if (replay_index >= (int)replay_manager_data->count)
            break;

        score_t entry = replay_manager_data->scores[replay_index];
        float y = y_start - i * 40;
        float x = 50;
        if (replay_index == replay_selected_index)
            x = 50 - 40*easing_get_factor();

        song_select_selector.y = y - 4;
        song_select_selector.x = x;

        sprite_draw(&song_select_selector, &song_select_selector_texture);

        text_renderer_draw(stringf("Score|Acc|Combo|Hit|Miss\n%d|%2.2f%%|%d|%d|%d",
                entry.total_score,
                entry.accuracy * 100.0f,
                entry.max_combo,
                entry.numPerfect + entry.numGreat + entry.numGood + entry.numOk + entry.numMeh,
                entry.numMiss),
            x+5, y+16, 8
        );
    }
}
