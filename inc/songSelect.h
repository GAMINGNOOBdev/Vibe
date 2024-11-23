#ifndef __SONGSELECT_H_
#define __SONGSELECT_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

void switchToSongSelect();

void songSelectInit();
void songSelectDispose();
void songSelectUpdate(float delta);
void songSelectRender();

#ifdef __cplusplus
}
#endif

#endif