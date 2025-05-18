#ifndef __GFX_H_
#define __GFX_H_ 1

#include <pspdisplay.h>
#include <pspgum.h>
#include <pspgu.h>

#define PSP_BUFFER_WIDTH 512
#define PSP_SCREEN_WIDTH 480
#define PSP_SCREEN_HEIGHT 272

#ifdef __cplusplus
extern "C" {
#endif

void graphics_init();
void graphics_dispose();
void graphics_start_frame();
void graphics_end_frame();

#ifdef __cplusplus
}
#endif

#endif
