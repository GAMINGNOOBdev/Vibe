#include <gfx.h>
#include <app.h>
#include <time.h>
#include <audio.h>
#include <input.h>
#include <options.h>
#include <text_renderer.h>
#ifdef __PSP__
#include <gu2gl.h>
#else
#include <GL/gl.h>
#include <pctypes.h>
#include <cglm/cglm.h>
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

#define NUM_OPTIONS 11

////////////////
///          ///
///   DATA   ///
///          ///
////////////////

int selected_option = 0;
uint8_t option_selected = 0;
uint32_t selected_color = 0xFFFFFFFF;

/////////////////////
///               ///
///   FUNCTIONS   ///
///               ///
/////////////////////

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
    glMatrixMode(GL_MODEL);
    glLoadIdentity();
    #endif

    LOGDEBUG("settings menu initialized");
}

void settings_menu_dispose(void)
{
    if (!settings_menu_initialized)
        return;
}

void settings_option_set_select_state(void)
{
    option_selected = !option_selected;
    if (option_selected)
        selected_color = 0xFF0000FF;
    else
        selected_color = 0xFFFFFFFF;
}

void settings_menu_handle_menu_movement(void)
{
    if (button_pressed_once(PSP_CTRL_UP) && selected_option > 0)
        selected_option--;
    if (button_pressed_once(PSP_CTRL_DOWN) && selected_option < NUM_OPTIONS)
        selected_option++;

    if (selected_option == NUM_OPTIONS)
        selected_option = NUM_OPTIONS-1;

    if (button_pressed_once(PSP_CTRL_CROSS))
        settings_option_set_select_state();
}

void settings_menu_handle_option_values(float delta)
{
    if (selected_option < 3)
    {
        float* val = &options.master_volume;
        if (selected_option == 1)
            val = &options.music_volume;
        else if (selected_option == 2)
            val = &options.hitsound_volume;

        if (button_pressed(PSP_CTRL_DOWN))
            *val -= delta * 1/10;
        if (button_pressed(PSP_CTRL_UP))
            *val += delta * 1/10;

        if (*val < 0)
            *val = 0;
        if (*val > 1)
            *val = 1;
    }
    if (selected_option == 3)
    {
        if (button_pressed_once(PSP_CTRL_DOWN) && options.scroll_speed > SCROLL_SPEED_MIN)
            options.scroll_speed--;
        if (button_pressed_once(PSP_CTRL_UP) && options.scroll_speed < SCROLL_SPEED_MAX)
            options.scroll_speed++;
    }
    if (selected_option > 3 && selected_option < 8)
    {
        int button = wait_for_input();
        if (button == -1)
            return;

        if (selected_option == 4)
            options.keybinds.m4l1 = button;
        if (selected_option == 5)
            options.keybinds.m4l2 = button;
        if (selected_option == 6)
            options.keybinds.m4l3 = button;
        if (selected_option == 7)
            options.keybinds.m4l4 = button;

        settings_option_set_select_state();
        return;
    }
    if (selected_option > 7)
    {
        int confirm = button_pressed_once(PSP_CTRL_CROSS);

        if (confirm && selected_option == 8)
            options.flags.show_fps = !options.flags.show_fps;
        if (confirm && selected_option == 9)
            options.flags.show_debug_info = !options.flags.show_debug_info;
        if (confirm && selected_option == 10)
            options.flags.enable_debug_log = !options.flags.enable_debug_log;
    }

    if (button_pressed_once(PSP_CTRL_CIRCLE))
        settings_option_set_select_state();
}

void settings_menu_input_handle(float delta)
{
    if (button_pressed_once(PSP_CTRL_CIRCLE) && !option_selected)
    {
        options_save();
        options_apply();
        switch_to_main_menu();
        return;
    }

    if (!option_selected)
        settings_menu_handle_menu_movement();
    else
        settings_menu_handle_option_values(delta);

    
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

    if (options.flags.show_fps)
        text_renderer_draw(stringf("%d fps", time_fps()), 0, 0, 8);

    text_renderer_draw_color("X -> Select option, O -> Go back\n\x18/\x19 -> Change selection/value", 5, 264, 8, 0xFF00FF00);

    int y = 248;
    text_renderer_draw_color(">", 5, y - selected_option*8, 8, selected_color);

    const char* keybind1 = get_psp_button_string(options.keybinds.m4l1);
    const char* keybind2 = get_psp_button_string(options.keybinds.m4l2);
    const char* keybind3 = get_psp_button_string(options.keybinds.m4l3);
    const char* keybind4 = get_psp_button_string(options.keybinds.m4l4);

    const char* options_string = stringf("Master volume: %2.2f%%\nMusic volume: %2.2f%%\nHitsound volume: %2.2f%%\nScroll speed: %d\nKeybind 1 4k: %s\nKeybind 2 4k: %s\nKeybind 3 4k: %s\nKeybind 4 4k: %s\nShow FPS: %s\nShow debug info: %s\nEnable debug log: %s",
                                         100.f * options.master_volume, 100.f * options.music_volume, 100.f * options.hitsound_volume, options.scroll_speed,
                                     keybind1, keybind2, keybind3, keybind4,
                                     options.flags.show_fps ? "true" : "false", options.flags.show_debug_info ? "true" : "false", options.flags.enable_debug_log ? "true" : "false");

    text_renderer_draw(options_string, 13, y, 8);
}
