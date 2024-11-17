#define GUGL_IMPLEMENTATION
#include <gfx.h>
#include <gu2gl.h>

static uint32_t __attribute__((aligned(16))) guDrawList[262144];

void initGraphics()
{
    guglInit(guDrawList);
}

void disposeGraphics()
{
    guglTerm();
}

void startFrame()
{
    guglStartFrame(guDrawList, GL_FALSE);
}

void endFrame()
{
    guglSwapBuffers(GL_TRUE, GL_FALSE);
}
