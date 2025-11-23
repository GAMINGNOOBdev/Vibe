#include <results_screen.h>
#include <text_renderer.h>
#include <song_select.h>
#include <logging.h>
#include <strutil.h>
#include <options.h>
#include <texture.h>
#include <sprite.h>
#include <gaming.h>
#include <input.h>
#include <time.h>
#include <app.h>
#include <gfx.h>

#ifndef __PSP__
#include <pctypes.h>
#else
#include <gu2gl.h>
#endif

results_screen_drawinfo_t results_drawinfo;
gaming_drawinfo_t results_gaming_drawinfo;
texture_t* results_ranking_texture;
replay_t* results_replay;
score_t results_score;
uint64_t results_map, results_set;
uint8_t results_saved_replay_data;
struct tm* replay_time = NULL;

void switch_to_results_screen(gaming_drawinfo_t* drawinfo, score_t* scoreptr, replay_t* replayptr, uint64_t map_id, uint64_t set_id)
{
    results_screen_init();

    app_set_update_callback(results_screen_update);
    app_set_render_callback(results_screen_render);

    results_saved_replay_data = 0;
    results_gaming_drawinfo = *drawinfo;
    results_score = *scoreptr;
    results_replay = replayptr;
    results_map = map_id;
    results_set = set_id;

    time_t local_time = time(NULL);
    replay_time = localtime(&local_time);

    const char* filename = stringf("Replays/%lld_%lld_%02d-%02d-%02d_%04d-%02d-%02d", map_id, set_id, replay_time->tm_hour, replay_time->tm_min, replay_time->tm_sec, replay_time->tm_year+1900, replay_time->tm_mon, replay_time->tm_mday);
    replay_save(NULL, scoreptr, 0, filename);

    if (results_score.accuracy == 1.f)
        results_ranking_texture = &results_drawinfo.rankX;
    else if (results_score.accuracy > 0.95f)
        results_ranking_texture = &results_drawinfo.rankS;
    else if (results_score.accuracy > 0.9f)
        results_ranking_texture = &results_drawinfo.rankA;
    else if (results_score.accuracy > 0.8f)
        results_ranking_texture = &results_drawinfo.rankB;
    else if (results_score.accuracy > 0.7f)
        results_ranking_texture = &results_drawinfo.rankC;
    else
        results_ranking_texture = &results_drawinfo.rankD;
}

uint8_t results_screen_initialized = 0;
void results_screen_init(void)
{
    if (results_screen_initialized)
        return;

    texture_load(&results_drawinfo.rankingPanelTexture, "Skin/ranking-panel.png", GL_TRUE, GL_FALSE);
    sprite_create(&results_drawinfo.rankingPanel, 0, 0, PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT, &results_drawinfo.rankingPanelTexture);

    texture_load(&results_drawinfo.rankA, "Skin/ranking-A-small.png", GL_TRUE, GL_FALSE);
    texture_load(&results_drawinfo.rankB, "Skin/ranking-B-small.png", GL_TRUE, GL_FALSE);
    texture_load(&results_drawinfo.rankC, "Skin/ranking-C-small.png", GL_TRUE, GL_FALSE);
    texture_load(&results_drawinfo.rankD, "Skin/ranking-D-small.png", GL_TRUE, GL_FALSE);
    texture_load(&results_drawinfo.rankS, "Skin/ranking-S-small.png", GL_TRUE, GL_FALSE);
    texture_load(&results_drawinfo.rankX, "Skin/ranking-X-small.png", GL_TRUE, GL_FALSE);
    sprite_create(&results_drawinfo.rank, 0, 0, 64, 64, &results_drawinfo.rankX);

    results_screen_initialized = 1;
}

void results_screen_dispose(void)
{
    if (!results_screen_initialized)
        return;

    texture_dispose(&results_drawinfo.rankA);
    texture_dispose(&results_drawinfo.rankB);
    texture_dispose(&results_drawinfo.rankC);
    texture_dispose(&results_drawinfo.rankD);
    texture_dispose(&results_drawinfo.rankS);
    texture_dispose(&results_drawinfo.rankX);
    sprite_dispose(&results_drawinfo.rank);

    texture_dispose(&results_drawinfo.rankingPanelTexture);
    sprite_dispose(&results_drawinfo.rankingPanel);
}

void results_screen_update(float delta)
{
    if (button_pressed(options.keybinds.back))
        switch_to_song_select();

    if (button_pressed_once(options.keybinds.start) && !results_saved_replay_data)
    {
        const char* filename = stringf("Replays/%lld_%lld_%02d-%02d-%02d_%04d-%02d-%02d", results_set, results_map, replay_time->tm_hour, replay_time->tm_min, replay_time->tm_sec, replay_time->tm_year+1900, replay_time->tm_mon, replay_time->tm_mday);
        replay_save(results_replay, &results_score, 1, filename);
        results_saved_replay_data = 1;
    }
}

void results_screen_render(void)
{
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

    sprite_draw(&results_drawinfo.rankingPanel, &results_drawinfo.rankingPanelTexture);

    const char* hitinfotextleft = stringf("%ix\n\n\n%ix\n\n\n%ix",
                                      results_score.numGreat,
                                      results_score.numGood,
                                      results_score.numMeh);
    text_renderer_draw(hitinfotextleft, 50, 180, 16);

    const char* hitinfotextright = stringf("%ix\n\n\n%ix\n\n\n%ix",
                                      results_score.numPerfect,
                                      results_score.numOk,
                                      results_score.numMiss);

    text_renderer_draw(hitinfotextright, 210, 180, 16);

    results_gaming_drawinfo.maniahit.x = 20;
    results_gaming_drawinfo.maniahit.y = 182;
    sprite_draw(&results_gaming_drawinfo.maniahit, &results_gaming_drawinfo.maniahit300_texture);
    results_gaming_drawinfo.maniahit.x = 20;
    results_gaming_drawinfo.maniahit.y = 134;
    sprite_draw(&results_gaming_drawinfo.maniahit, &results_gaming_drawinfo.maniahit200_texture);
    results_gaming_drawinfo.maniahit.x = 20;
    results_gaming_drawinfo.maniahit.y = 86;
    sprite_draw(&results_gaming_drawinfo.maniahit, &results_gaming_drawinfo.maniahit50_texture);
    results_gaming_drawinfo.maniahit.x = 180;
    results_gaming_drawinfo.maniahit.y = 182;
    sprite_draw(&results_gaming_drawinfo.maniahit, &results_gaming_drawinfo.maniahit300g_texture);
    results_gaming_drawinfo.maniahit.x = 180;
    results_gaming_drawinfo.maniahit.y = 134;
    sprite_draw(&results_gaming_drawinfo.maniahit, &results_gaming_drawinfo.maniahit100_texture);
    results_gaming_drawinfo.maniahit.x = 180;
    results_gaming_drawinfo.maniahit.y = 86;
    sprite_draw(&results_gaming_drawinfo.maniahit, &results_gaming_drawinfo.maniahit0_texture);

    const char* scoreinfotext = stringf("%7.7i",
                                        results_score.total_score);
    text_renderer_draw(scoreinfotext, 90, PSP_SCREEN_HEIGHT-35, 16);
    text_renderer_draw(stringf("%2.2f%%", results_score.accuracy * 100.f), 165, 20, 16);
    text_renderer_draw(stringf("%ix", results_score.max_combo), 15, 20, 16);

    if (results_saved_replay_data)
        text_renderer_draw("Replay saved", 280, 60, 16);
    else
        text_renderer_draw("Save replay", 280, 60, 16);

    results_drawinfo.rank.x = 380;
    results_drawinfo.rank.y = 172;
    sprite_draw(&results_drawinfo.rank, results_ranking_texture);
}
