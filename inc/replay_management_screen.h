#ifndef __REPLAY_MANAGEMENT_SCREEN_H_
#define __REPLAY_MANAGEMENT_SCREEN_H_

#include <stdint.h>

void switch_to_replay_management_screen(uint64_t map_id, uint64_t set_id);

void replay_management_screen_update(float delta);
void replay_management_screen_render(void);

#endif
