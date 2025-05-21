#include "text_renderer.h"
#include <gfx.h>
#include <app.h>
#include <audio.h>
#include <input.h>
#ifdef __PSP__
#include <gu2gl.h>
#else
#include <GL/gl.h>
#include <pctypes.h>
#endif
#include <sprite.h>
#include <texture.h>
#include <logging.h>
#include <main_menu.h>
#include <song_select.h>
#include <settings_menu.h>

#ifdef __PSP__
#include <pspgu.h>
#include <pspgum.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspaudio.h>
#endif

////////////////
///          ///
///   DATA   ///
///          ///
////////////////

texture_t* main_menu_background_texture;
sprite_t* main_menu_background;

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
    #ifdef __PSP__
    glMatrixMode(GL_VIEW);
    glLoadIdentity();
    #endif
    glMatrixMode(GL_MODEL);
    glLoadIdentity();

    main_menu_background_texture = texture_load("Assets/mainMenu.png", GL_TRUE, GL_TRUE);
    main_menu_background = sprite_create(0, 0, 480, 272, main_menu_background_texture);

    LOGDEBUG("main menu initialized");
}

void main_menu_dispose(void)
{
    if (!menu_initialized)
        return;

    texture_dispose(main_menu_background_texture);
    sprite_dispose(main_menu_background);
}

void main_menu_input_handle(float delta)
{
    if (button_pressed_once(PSP_CTRL_START))
        switch_to_song_select();

    if (button_pressed_once(PSP_CTRL_SELECT))
        switch_to_settings_menu();
}

void main_menu_update(float delta)
{
    main_menu_input_handle(delta);
}

void main_menu_render(void)
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

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, PSP_SCREEN_WIDTH, 0, PSP_SCREEN_HEIGHT, -0.01f, 10.0f);

    #ifndef __PSP__
    graphics_projection_matrix();
    #endif

    glEnable(GL_TEXTURE_2D);
    sprite_draw(main_menu_background, main_menu_background_texture);

    text_renderer_draw("Press Start to start", 160, 20, 8);
    text_renderer_draw("Press Select to configure settings", 104, 12, 8);
}
