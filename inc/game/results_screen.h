#ifndef __GAME__RESULTS_SCREEN_H_
#define __GAME__RESULTS_SCREEN_H_ 1

#include "engine/texture.h"
#include "engine/sprite.h"

#include "scoring.h"
#include "gaming.h"
#include "replay.h"

typedef struct
{
    texture_t rankingPanelTexture;
    texture_t ranksTexture;
    sprite_t rankingPanel;
    sprite_t rank;
} results_screen_drawinfo_t;

void switch_to_results_screen(gaming_drawinfo_t* drawinfo, score_t* scoreptr, replay_t* replayptr, uint64_t map_id, uint64_t set_id);

void results_screen_init(void);
void results_screen_dispose(void);
void results_screen_update(float delta);
void results_screen_render(void);

#endif
