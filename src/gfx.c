#include <gfx.h>

uint32_t __attribute__((aligned(16))) drawCommandList[262144];

uint32_t getMemorySize(uint32_t width, uint32_t height, uint32_t psm)
{
    uint32_t size = width * height;

    switch (psm)
    {
        case GU_PSM_T4:
            return size/2;
        
        case GU_PSM_T8:
            return size;
        
        case GU_PSM_5650:
        case GU_PSM_5551:
        case GU_PSM_4444:
        case GU_PSM_T16:
            return size * 2;
        
        case GU_PSM_8888:
        case GU_PSM_T32:
            return size * 4;
        
        default:
            return 0;
    }
}

void* getStaticVramBuffer(uint32_t width, uint32_t height, uint32_t psm)
{
    static uint32_t staticOffset = 0;

    uint32_t memSize = getMemorySize(width, height, psm);

    void* result = (void*)staticOffset;

    staticOffset += memSize;

    return result;
}

void* getStaticVramTexture(uint32_t width, uint32_t height, uint32_t psm)
{
    void* result = getStaticVramBuffer(width, height, psm);
    return (void*)( ((uint32_t)result) + ((uint32_t)sceGeEdramGetAddr()) );
}


void initGraphics()
{
    void* framebuffer0 = getStaticVramBuffer(PSP_BUFFER_WIDTH, PSP_SCREEN_HEIGHT, GU_PSM_8888);
    void* framebuffer1 = getStaticVramBuffer(PSP_BUFFER_WIDTH, PSP_SCREEN_HEIGHT, GU_PSM_8888);
    void* zBuffer = getStaticVramBuffer(PSP_BUFFER_WIDTH, PSP_SCREEN_HEIGHT, GU_PSM_4444);

    sceGuInit();

    sceGuStart(GU_DIRECT, drawCommandList);
    sceGuDrawBuffer(GU_PSM_8888, framebuffer0, PSP_BUFFER_WIDTH);
    sceGuDispBuffer(PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT, framebuffer0, PSP_BUFFER_WIDTH);
    sceGuDepthBuffer(zBuffer, PSP_BUFFER_WIDTH);

    sceGuOffset(2048 - (PSP_SCREEN_WIDTH/2), 2048 - (PSP_SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT);

    sceGuDepthRange(65535, 0);

    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, PSP_SCREEN_WIDTH, PSP_SCREEN_WIDTH);

    sceGuEnable(GU_DEPTH_TEST);
    sceGuDepthFunc(GU_GEQUAL);

    sceGuFrontFace(GU_CW);
    sceGuEnable(GU_CULL_FACE);

    sceGuShadeModel(GU_SMOOTH);

    sceGuEnable(GU_TEXTURE_2D);
    sceGuEnable(GU_CLIP_PLANES);

    sceGuFinish();
    sceGuSync(0, 0);

    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);
}

void disposeGraphics()
{
    sceGuTerm();
}

void startFrame()
{
    sceGuStart(GU_DIRECT, drawCommandList);
}

void endFrame()
{
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}
