#ifndef __RESULTS_SCREEN_H_
#define __RESULTS_SCREEN_H_ 1

#include <texture.h>
#include <scoring.h>
#include <gaming.h>
#include <sprite.h>

typedef struct
{
    texture_t rankA, rankB, rankC, rankD, rankS, rankX;
    texture_t rankingPanelTexture;
    sprite_t rankingPanel;
    sprite_t rank;
} results_screen_drawinfo_t;

void switch_to_results_screen(gaming_drawinfo_t* drawinfo, score_t* scoreptr);

void results_screen_init(void);
void results_screen_dispose(void);
void results_screen_update(float delta);
void results_screen_render(void);

#endif
