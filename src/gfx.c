#define GUGL_IMPLEMENTATION
#include <gfx.h>
#include <gu2gl.h>

static uint32_t __attribute__((aligned(16))) guDrawList[262144];

void graphics_init()
{
    guglInit(guDrawList);
}

void graphics_dispose()
{
    guglTerm();
}

void graphics_start_frame()
{
    guglStartFrame(guDrawList, GL_FALSE);
}

void graphics_end_frame()
{
    guglSwapBuffers(GL_TRUE, GL_FALSE);
}
