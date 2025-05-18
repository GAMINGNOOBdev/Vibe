#include "text_renderer.h"
#include <gfx.h>
#include <app.h>
#include <audio.h>
#include <input.h>
#include <gu2gl.h>
#include <pspgu.h>
#include <sprite.h>
#include <pspgum.h>
#include <texture.h>
#include <logging.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspaudio.h>
#include <main_menu.h>
#include <song_select.h>

////////////////
///          ///
///   DATA   ///
///          ///
////////////////

texture_t main_menu_background_texture;
sprite_t main_menu_background;

/////////////////////
///               ///
///   FUNCTIONS   ///
///               ///
/////////////////////

void switch_to_main_menu(void)
{
    main_menu_init();

    app_set_update_callback(main_menu_update);
    app_set_render_callback(main_menu_render);

    LOGINFO("loaded main menu");
}

uint8_t menu_initialized = 0;
void main_menu_init(void)
{
    if (menu_initialized)
        return;

    menu_initialized = 1;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_VIEW);
    glLoadIdentity();
    glMatrixMode(GL_MODEL);
    glLoadIdentity();

    texture_load(&main_menu_background_texture, "Assets/mainMenu.png", GL_TRUE, GL_TRUE);
    sprite_create(&main_menu_background, 0, 0, 480, 272, &main_menu_background_texture);

    LOGDEBUG("main menu initialized");
}

void main_menu_dispose(void)
{
    if (!menu_initialized)
        return;

    sprite_dispose(&main_menu_background);
}

void main_menu_input_handle(float delta)
{
    if (button_pressed(PSP_CTRL_START))
        switch_to_song_select();
}

void main_menu_update(float delta)
{
    main_menu_input_handle(delta);
}

void main_menu_render(void)
{
    glDisable(GL_DEPTH_TEST);

    glBlendFunc(GL_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 0, 0);
    glEnable(GL_BLEND);

    glClearColor(0xFF111111);
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
    sprite_draw(&main_menu_background, &main_menu_background_texture);

    text_renderer_draw("Press Start to start", 160, 20, 8);
}
