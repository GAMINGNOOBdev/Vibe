#include <gfx.h>
#include <app.h>
#include <audio.h>
#include <input.h>
#include <options.h>
#include <text_renderer.h>
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

/////////////////////
///               ///
///   FUNCTIONS   ///
///               ///
/////////////////////

const char* get_psp_button_string(int button)
{
    if (button == PSP_CTRL_SELECT)
        return "SELECT";
    if (button == PSP_CTRL_START)
        return "START";
    if (button == PSP_CTRL_UP)
        return "\x18";
    if (button == PSP_CTRL_RIGHT)
        return "\x1A";
    if (button == PSP_CTRL_DOWN)
        return "\x19";
    if (button == PSP_CTRL_LEFT)
        return "\x1B";
    if (button == PSP_CTRL_LTRIGGER)
        return "L";
    if (button == PSP_CTRL_RTRIGGER)
        return "R";
    if (button == PSP_CTRL_TRIANGLE)
        return "\x1E";
    if (button == PSP_CTRL_CIRCLE)
        return "O";
    if (button == PSP_CTRL_CROSS)
        return "X";
    if (button == PSP_CTRL_SQUARE)
        return "\xA";

    return "None";
}

void switch_to_settings_menu(void)
{
    settings_menu_init();

    app_set_update_callback(settings_menu_update);
    app_set_render_callback(settings_menu_render);

    LOGINFO("loaded settings menu");
}

uint8_t settings_menu_initialized = 0;
void settings_menu_init(void)
{
    if (settings_menu_initialized)
        return;

    settings_menu_initialized = 1;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    #ifdef __PSP__
    glMatrixMode(GL_VIEW);
    glLoadIdentity();
    #endif
    glMatrixMode(GL_MODEL);
    glLoadIdentity();

    LOGDEBUG("settings menu initialized");
}

void settings_menu_dispose(void)
{
    if (!settings_menu_initialized)
        return;
}

void settings_menu_input_handle(float delta)
{
    if (button_pressed_once(PSP_CTRL_SELECT))
    {
        options_save();
        switch_to_main_menu();
    }
}

void settings_menu_update(float delta)
{
    settings_menu_input_handle(delta);
}

void settings_menu_render(void)
{
    glDisable(GL_DEPTH_TEST);

    #ifdef __PSP__
    glBlendFunc(GL_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 0, 0);
    glEnable(GL_BLEND);

    glClearColor(0xFF000000);
    #else
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0x00/255.f, 0x00/255.f, 0x00/255.f, 0xFF/255.f);
    #endif

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, PSP_SCREEN_WIDTH, 0, PSP_SCREEN_HEIGHT, -0.01f, 10.0f);

    #ifndef __PSP__
    graphics_projection_matrix();
    #endif

    text_renderer_draw("START -> change option value, SELECT -> Go back", 0, 264, 8);
    text_renderer_draw("\x18/\x19 -> change selection", 0, 256, 8);

    int y = 248;
    ///TODO: make this work    
}
