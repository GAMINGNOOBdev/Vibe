#ifndef __SONG_SELECT_H_
#define __SONG_SELECT_H_ 1

void switch_to_song_select(void);

void song_select_init(void);
void song_select_dispose(void);
void song_select_update(float delta);
void song_select_render(void);

#endif
