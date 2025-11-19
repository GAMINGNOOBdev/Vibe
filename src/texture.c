#ifdef __PSP__
#include <pspkernel.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <texture.h>
#include <stb_image.h>
#include <logging.h>
#include <memory.h>
#include <stdlib.h>
#ifdef __PSP__
#include <gu2gl.h>
#else
#include <gfx.h>
#include <GL/glew.h>
#include <pctypes.h>
#endif

uint32_t pow2(uint32_t value)
{
    uint32_t result = 1;
    while (result < value)
        result <<= 1;

    return result;
}

void copy_tex_data(void* dest, const void* src, const int pW, const int width, const int height)
{
    for (uint32_t y = 0; y < height; y++)
        for (uint32_t x = 0; x < width; x++)
            ((uint32_t*)dest)[x + y*pW] = ((uint32_t*)src)[x + y*width];

}

void swizzle_fast(uint8_t* out, const uint8_t* in, const uint32_t width, const uint32_t height)
{
    uint32_t blockx, blocky;
    uint32_t j;
    uint32_t blockWidth = (width / 16);
    uint32_t blockHeight = (height / 8);

    uint32_t srcPitch = (width - 16) / 4;
    uint32_t srcRow = width*8;

    const uint8_t* ysrc = in;
    uint32_t* dest = (uint32_t*)out;

    for (blocky = 0; blocky < blockHeight; ++blocky)
    {
        const uint8_t* xsrc = ysrc;
        for (blockx = 0; blockx < blockWidth; ++blockx)
        {
            const uint32_t* src = (uint32_t*)xsrc;
            for (j = 0; j < 8; j++)
            {
                *(dest++) = *(src++);
                *(dest++) = *(src++);
                *(dest++) = *(src++);
                *(dest++) = *(src++);
                src += srcPitch;
            }
            xsrc += 16;
        }
        ysrc += srcRow;
    }
}

void texture_load(texture_t* texture, const char* filename, const int flip, const int vram)
{
    if (!texture || !filename)
    {
        LOGERROR("could not allocate texture object for file '%s'", filename);
        return;
    }

    int width, height, channels;
    stbi_set_flip_vertically_on_load(flip);
    uint8_t* imgData = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

    if (!imgData)
    {
        LOGERROR("could not load texture '%s'", filename);
        return;
    }
    texture->width = width;
    texture->height = height;
    texture->pWidth = width;
    texture->pHeight = height;
    #ifndef __PSP__
    size_t size = texture->pWidth * texture->pHeight * 4;

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
    stbi_image_free(imgData);

    LOGDEBUG("texture id: %d (0x%16.16x)", texture->id, texture->id);
    #else
    texture->pWidth = pow2(width);
    texture->pHeight = pow2(height);

    size_t size = texture->pWidth * texture->pHeight * 4;


    uint32_t* dataBuffer = (uint32_t*)memalign(16, size);
    if (!dataBuffer)
    {
        stbi_image_free(imgData);
        LOGERROR("could not allocate data for texture '%s'", filename);
        return;
    }

    copy_tex_data(dataBuffer, imgData, texture->pWidth, width, height);
    stbi_image_free(imgData);

    uint32_t* swizzledPixels = NULL;
    if (vram)
        swizzledPixels = (uint32_t*)getStaticVramTexture(texture->pWidth, texture->pHeight, GL_PSM_8888);
    else
        swizzledPixels = (uint32_t*)memalign(16, size);

    if (!swizzledPixels)
    {
        LOGERROR("could not allocate data to swizzle texture '%s'", filename);
        return;
    }

    swizzle_fast((uint8_t*)swizzledPixels, (const uint8_t*)dataBuffer, texture->pWidth * 4, texture->pHeight);

    texture->data = swizzledPixels;
    free(dataBuffer);

    sceKernelDcacheWritebackInvalidateAll();
    #endif

    LOGINFO("texture '%s' with size '%ld' (%ldx%ld or %ldx%ld) loaded into %cram", filename, size, width, height, texture->pWidth, texture->pHeight, vram ? 'v' : ' ');
}

void texture_bind(texture_t* tex)
{
    #ifdef __PSP__
    if (!tex)
        return;

    glTexMode(GL_PSM_8888, 0, 0, GL_TRUE);
    glTexFunc(GL_TFX_MODULATE, GL_TCC_RGBA); // output = vertex * texture
    glTexFilter(GL_NEAREST, GL_NEAREST);
    glTexWrap(GL_REPEAT, GL_REPEAT);
    glTexImage(0, tex->pWidth, tex->pHeight, tex->pWidth, tex->data);
    #else
    if (!tex)
    {
        graphics_no_texture();
        return;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->id);
    graphics_texture_uniform(0);
    #endif
}

void texture_dispose(texture_t* tex)
{
    if (!tex)
        return;

    #ifndef __PSP__
    glDeleteTextures(1, &tex->id);
    #endif
}

void texture_atlas_get_uv_index(texture_atlas_t* atlas, float* buffer, int index)
{
    int row = index / (int)atlas->width;
    int column = index % (int)atlas->height;

    float sizeX = 1.f / ((float)atlas->width);
    float sizeY = 1.f / ((float)atlas->height);

    float y = (float)row * sizeY;
    float x = (float)column * sizeX;

    float h = y + sizeY;
    float w = x + sizeX;

    buffer[0] = x;
    buffer[1] = h;

    buffer[2] = x;
    buffer[3] = y;

    buffer[4] = w;
    buffer[5] = y;

    buffer[6] = w;
    buffer[7] = h;
}
