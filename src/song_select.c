#include <text_renderer.h>
#include <song_select.h>
#include <song_list.h>
#include <main_menu.h>
#include <options.h>
#include <texture.h>
#include <logging.h>
#include <easing.h>
#include <gaming.h>
#include <sprite.h>
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

#define SONGS_ON_SCREEN 6
#define DIFFICULTIES_ON_SCREEN 5

typedef enum {
    SONG_SELECT_STATE_LIST,
    SONG_SELECT_STATE_DIFFICULTY_SELECT
} song_select_state_t;

////////////////
///          ///
///   DATA   ///
///          ///
////////////////

song_select_state_t song_select_state = SONG_SELECT_STATE_LIST;
int song_selected_index = 0;
int song_scroll_offset = 0;

int difficulty_selected_index = 0;
int difficulty_scroll_offset = 0;

song_difficulty_t* selected_difficulty = NULL;
songlist_entry_t* selected_song = NULL;

sprite_t song_select_background;
sprite_t song_select_selector;

texture_t song_select_background_texture;
texture_t song_select_selector_texture;

/////////////////////
///               ///
///   FUNCTIONS   ///
///               ///
/////////////////////

void switch_to_song_select(void)
{
    song_select_init();

    app_set_update_callback(song_select_update);
    app_set_render_callback(song_select_render);

    audio_set_end_callback(NULL);
    audio_set_stream(NULL);

    easing_enable();
    easing_reset_timer();
    easing_set_duration(0.5f);
    easing_set_type(easeOutCubic);

    LOGINFO("loaded song select");
}

uint8_t song_select_initialized = 0;
void song_select_init(void)
{
    if (song_select_initialized)
        return;

    song_list_initialize("Songs");

    texture_load(&song_select_background_texture, "Assets/background.png", 1, 1);
    texture_load(&song_select_selector_texture, "Assets/selector.png", 0, 1);

    sprite_create(&song_select_background, 0, 0, PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT, &song_select_background_texture);
    sprite_create(&song_select_selector, 50, 222, 460, 30, &song_select_selector_texture);

    song_select_initialized = 1;
}

void song_select_dispose(void)
{
    if (!song_select_initialized)
        return;

    texture_dispose(&song_select_background_texture);
    texture_dispose(&song_select_selector_texture);
    sprite_dispose(&song_select_background);
    sprite_dispose(&song_select_selector);
}

void song_select_input_handle(float delta)
{
    int confirm = button_pressed_once(PSP_CTRL_CROSS);
    int back = button_pressed_once(PSP_CTRL_CIRCLE);
    int down = button_pressed_once(PSP_CTRL_DOWN);
    int up = button_pressed_once(PSP_CTRL_UP);

    if (song_select_state == SONG_SELECT_STATE_LIST)
    {
        if (back)
            switch_to_main_menu();

        if (up && song_selected_index > 0)
        {
            song_selected_index--;
            if (song_selected_index < song_scroll_offset)
                song_scroll_offset--;

            easing_reset_timer();
            easing_set_duration(0.5f);
            easing_set_type(easeOutCubic);
        }
        else if (up && song_selected_index == 0)
        {
            song_selected_index = songs_list.count-1;
            if (song_selected_index >= song_scroll_offset + SONGS_ON_SCREEN)
                song_scroll_offset = song_selected_index - SONGS_ON_SCREEN + 1;

            easing_reset_timer();
            easing_set_duration(0.5f);
            easing_set_type(easeOutCubic);
        }
        if (down && song_selected_index < (int)songs_list.count - 1)
        {
            song_selected_index++;
            if (song_selected_index >= song_scroll_offset + SONGS_ON_SCREEN)
                song_scroll_offset++;

            easing_reset_timer();
            easing_set_duration(0.5f);
            easing_set_type(easeOutCubic);
        }
        else if (down && song_selected_index == (int)songs_list.count - 1)
        {
            song_selected_index = 0;
            song_scroll_offset = 0;

            easing_reset_timer();
            easing_set_duration(0.5f);
            easing_set_type(easeOutCubic);
        }
        if (confirm && song_selected_index < (int)songs_list.count)
        {
            selected_song = songlist_get_entry(song_selected_index);
            song_select_state = SONG_SELECT_STATE_DIFFICULTY_SELECT;
            easing_reset_timer();
            easing_set_duration(1.5f);
            easing_set_type(easeOutCubic);
        }
    }
    else if (song_select_state == SONG_SELECT_STATE_DIFFICULTY_SELECT)
    {
        if (back)
        {
            song_select_state = SONG_SELECT_STATE_LIST;
            difficulty_selected_index = 0;
            difficulty_scroll_offset = 0;
            selected_difficulty = NULL;
        }

        if (up && difficulty_selected_index > 0)
        {
            difficulty_selected_index--;
            if (difficulty_selected_index < difficulty_scroll_offset)
                difficulty_scroll_offset--;
        }
        else if (up && difficulty_selected_index == 0)
        {
            difficulty_selected_index = selected_song->difficulties.count - 1;
            if (difficulty_selected_index >= difficulty_scroll_offset + DIFFICULTIES_ON_SCREEN)
                difficulty_scroll_offset = difficulty_selected_index - DIFFICULTIES_ON_SCREEN + 1;
        }
        if (down && difficulty_selected_index < (int)selected_song->difficulties.count - 1)
        {
            difficulty_selected_index++;
            if (difficulty_selected_index >= difficulty_scroll_offset + DIFFICULTIES_ON_SCREEN)
                difficulty_scroll_offset++;
        }
        else if (down && difficulty_selected_index == (int)selected_song->difficulties.count - 1)
        {
            difficulty_selected_index = 0;
            difficulty_scroll_offset = 0;
        }
        if (confirm && difficulty_selected_index < (int)selected_song->difficulties.count)
        {
            LOGDEBUG(stringf("song{index: %d scroll: %d} difficulty{index: %d scroll: %d}",
                song_selected_index, song_scroll_offset, difficulty_selected_index, difficulty_scroll_offset));
            selected_difficulty = &selected_song->difficulties.data[difficulty_selected_index];
            switch_to_gaming(selected_song->fullname, selected_difficulty->filename);
        }
    }
}

void song_select_update(float delta)
{
    song_select_input_handle(delta);
    easing_update(delta);
}

void song_select_render(void)
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

    if (song_select_state == SONG_SELECT_STATE_LIST)
    {
        float y_start = 242;
        for (int i = 0; i < SONGS_ON_SCREEN; i++)
        {
            int song_index = song_scroll_offset + i;

            if (song_index >= (int)songs_list.count)
                break;

            songlist_entry_t* entry = songlist_get_entry(song_index);
            float y = y_start - i * 40;
            float x = 50;
            if (song_index == song_selected_index)
                x = 50 - 40*easing_get_factor();

            song_select_selector.y = y - 4;
            song_select_selector.x = x;

            sprite_draw(&song_select_selector, &song_select_selector_texture);

            text_renderer_draw(stringf("%s - %s\nID: %llu", entry->artist, entry->songname, entry->id), x+5, y+16, 8);
        }
    }
    else if (song_select_state == SONG_SELECT_STATE_DIFFICULTY_SELECT)
    {
        float y = 242 - (song_selected_index % SONGS_ON_SCREEN) * 40;
        float distanceToTop = 246-y;
        y = y + distanceToTop * easing_get_factor();
        float x = 10;
        song_select_selector.x = x;
        song_select_selector.y = y-4;

        sprite_draw(&song_select_selector, &song_select_selector_texture);
        text_renderer_draw(stringf("%s - %s\nID: %llu", selected_song->artist, selected_song->songname, selected_song->id), x+5, y+16, 8);

        float y_start = y-40;
        for (int i = 0; i < DIFFICULTIES_ON_SCREEN; i++)
        {
            int difficulty_index = difficulty_scroll_offset + i;

            if (difficulty_index >= (int)selected_song->difficulties.count)
                break;

            song_difficulty_t entry = selected_song->difficulties.data[difficulty_index];
            float y = y_start - i * 40;
            float x = 50;
            if (difficulty_index == difficulty_selected_index)
                x = 20;

            song_select_selector.y = y - 4;
            song_select_selector.x = x;

            sprite_draw(&song_select_selector, &song_select_selector_texture);
            text_renderer_draw(stringf("%s (%s)", entry.name, entry.mapper), x+5, y+8, 8);
        }
    }
}
