#ifndef __MANIAHIT_DISPALY_H_
#define __MANIAHIT_DISPALY_H_

#include <scoring.h>

void maniahit_display_init(void);
void maniahit_display_update(float delta);
void maniahit_display_render();
void maniahit_display_judement(scoring_judgement_type_t judgement);
void maniahit_display_at(float x, float y, scoring_judgement_type_t judgement);
void maniahit_display_dispose(void);

#endif