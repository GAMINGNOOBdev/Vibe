#include <gfx.h>
#include <app.h>
#include <input.h>
#include <strutil.h>
#include <options.h>
#include <time_util.h>
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
    #ifdef __PSP__
    glMatrixMode(GL_VIEW);
    glLoadIdentity();
    glMatrixMode(GL_MODEL);
    glLoadIdentity();
    #endif

    texture_load(&main_menu_background_texture, "Assets/mainMenu.png", GL_TRUE, GL_TRUE);
    sprite_create(&main_menu_background, 0, 0, 480, 272, &main_menu_background_texture);

    LOGDEBUG("main menu initialized");
}

void main_menu_dispose(void)
{
    if (!menu_initialized)
        return;

    texture_dispose(&main_menu_background_texture);
    sprite_dispose(&main_menu_background);
}

void main_menu_input_handle(float _)
{
    if (button_pressed_once(options.keybinds.start))
        switch_to_song_select();

    if (button_pressed_once(options.keybinds.select))
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

    sprite_draw(&main_menu_background, &main_menu_background_texture);

    if (options.flags.show_fps)
        text_renderer_draw(stringf("%d fps", time_fps()), 0, 0, 8);

    text_renderer_draw("Press Start to start", 160, 20, 8);
    text_renderer_draw("Press Select to configure settings", 104, 12, 8);
}
