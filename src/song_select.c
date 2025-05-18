#include "easing.h"
#include "text_renderer.h"
#include <song_select.h>
#include <song_list.h>
#include <main_menu.h>
#include <options.h>
#include <texture.h>
#include <logging.h>
#include <sprite.h>
#include <audio.h>
#include <input.h>
#include <gu2gl.h>
#include <pspgu.h>
#include <gfx.h>
#include <app.h>

#define SONGS_ON_SCREEN 6

typedef enum {
    SONG_SELECT_STATE_LIST,
    SONG_SELECT_STATE_DETAILS
} song_select_state_t;

song_select_state_t song_select_state;
int selected_index;
int scroll_offset;

songlist_entry_t* current;
songlist_entry_t* selected_song;

sprite_t song_select_background;
sprite_t song_select_selector;

texture_t song_select_background_texture;
texture_t song_select_selector_texture;

void song_select_music_end()
{
    ///TODO: --- implement ---
}

void switch_to_song_select(void)
{
    song_select_init();

    app_set_update_callback(song_select_update);
    app_set_render_callback(song_select_render);

    audio_set_end_callback(song_select_music_end);
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

    song_list_initialize("Songs/");

    song_select_state = SONG_SELECT_STATE_LIST;
    selected_index = 0;
    scroll_offset = 0;
    current = songs_list.start;
    selected_song = NULL;

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

        if (up && selected_index > 0)
        {
            selected_index--;
            if (selected_index < scroll_offset)
                scroll_offset--;

            easing_reset_timer();
            easing_set_duration(0.5f);
            easing_set_type(easeOutCubic);
        }
        if (down && selected_index < (int)songs_list.count - 1) {
            selected_index++;
            if (selected_index >= scroll_offset + SONGS_ON_SCREEN)
                scroll_offset++;

            easing_reset_timer();
            easing_set_duration(0.5f);
            easing_set_type(easeOutCubic);
        }
        if (confirm)
        {
            selected_song = songlist_get_entry(selected_index);
            song_select_state = SONG_SELECT_STATE_DETAILS;
            easing_reset_timer();
            easing_set_duration(1.5f);
            easing_set_type(easeOutCubic);
        }
    } else if (song_select_state == SONG_SELECT_STATE_DETAILS)
    {
        if (back)
            song_select_state = SONG_SELECT_STATE_LIST;

        // Handle difficulty selection input here (placeholder)
        ///TODO: --- show difficulties ---
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
}

void song_select_update(float delta)
{
    song_select_input_handle(delta);
    easing_update(delta);
}

void song_select_render(void)
{
    glDisable(GL_DEPTH_TEST);

    glBlendFunc(GL_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 0, 0);
    glEnable(GL_BLEND);

    glClearColor(0xFF333333);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ///////////////////
    ///             ///
    ///   UI PASS   ///
    ///             ///
    ///////////////////

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, PSP_SCREEN_WIDTH, 0, PSP_SCREEN_HEIGHT, -0.01f, 10.0f);

    glEnable(GL_TEXTURE_2D);
    sprite_draw(&song_select_background, &song_select_background_texture);

    if (song_select_state == SONG_SELECT_STATE_LIST)
    {
        float y_start = 222;
        for (int i = 0; i < SONGS_ON_SCREEN; i++)
        {
            int song_index = scroll_offset + i;

            if (song_index >= (int)songs_list.count)
                break;

            songlist_entry_t* entry = songlist_get_entry(song_index);
            float y = y_start - i * 40;
            float x = 50;
            if (song_index == selected_index)
                x = 50 - 30*easing_get_factor();

            song_select_selector.y = y - 4;
            song_select_selector.x = x;

            sprite_draw(&song_select_selector, &song_select_selector_texture);

            text_renderer_draw(stringf("%s - %s", entry->artist, entry->songname), x+5, y+16, 8);
            text_renderer_draw(stringf("ID: %llu", entry->id), x+5, y, 6);
        }
    }
    else if (song_select_state == SONG_SELECT_STATE_DETAILS)
    {
        float y = 222 - (selected_index % SONGS_ON_SCREEN) * 40;
        float x = 20 - 100.f * easing_get_factor();
        song_select_selector.x = x;
        song_select_selector.y = y-4;
        sprite_draw(&song_select_selector, &song_select_selector_texture);

        text_renderer_draw(stringf("%s - %s", selected_song->artist, selected_song->songname), x+5, y+16, 8);
        text_renderer_draw(stringf("ID: %llu", selected_song->id), x+5, y, 6);

        // Placeholder difficulty options
        text_renderer_draw("▶ Easy",   180, 180, 8);
        text_renderer_draw("   Normal", 180, 160, 8);
        text_renderer_draw("   Hard",   180, 140, 8);
    }
}

