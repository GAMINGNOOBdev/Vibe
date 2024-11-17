#ifndef __GFX_H_
#define __GFX_H_ 1

#include <pspdisplay.h>
#include <stdint.h>
#include <pspgum.h>
#include <pspgu.h>

#define PSP_BUFFER_WIDTH 512
#define PSP_SCREEN_WIDTH 480
#define PSP_SCREEN_HEIGHT 272

#ifdef __cplusplus
extern "C"
{
#endif

uint32_t getMemorySize(uint32_t width, uint32_t height, uint32_t psm);
void* getStaticVramBuffer(uint32_t width, uint32_t height, uint32_t psm);
void* getStaticVramTexture(uint32_t width, uint32_t height, uint32_t psm);

void initGraphics();
void disposeGraphics();
void startFrame();
void endFrame();

#ifdef __cplusplus
}
#endif

#endif