#define STB_IMAGE_IMPLEMENTATION
#include <texture.h>
#include <stb_image.h>
#include <pspkernel.h>
#include <logging.h>
#include <memory.h>
#include <malloc.h>
#include <gu2gl.h>

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

texture_t* loadTexture(const char* filename, const int flip, const int vram)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(flip);
    uint8_t* imgData = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

    if (!imgData)
    {
        LOGERROR(stringf("Could not load texture '%s'", filename));
        return NULL;
    }

    texture_t* result = (texture_t*)malloc(sizeof(texture_t));
    if (!result)
    {
        LOGERROR(stringf("Could not allocate texture object for file '%s'", filename));
        stbi_image_free(imgData);
        return NULL;
    }
    result->width = width;
    result->height = height;
    result->pWidth = pow2(width);
    result->pHeight = pow2(height);

    size_t size = result->pWidth * result->pHeight * 4;

    uint32_t* dataBuffer = (uint32_t*)memalign(16, size);
    if (!dataBuffer)
    {
        free(result);
        stbi_image_free(imgData);
        LOGERROR(stringf("Could not allocate data for texture '%s'", filename));
        return NULL;
    }

    copy_tex_data(dataBuffer, imgData, result->pWidth, width, height);
    stbi_image_free(imgData);

    uint32_t* swizzledPixels = NULL;
    if (vram)
        swizzledPixels = (uint32_t*)getStaticVramTexture(result->pWidth, result->pHeight, GL_PSM_8888);
    else
        swizzledPixels = (uint32_t*)memalign(16, size);

    if (!swizzledPixels)
    {
        LOGERROR(stringf("Could not allocate data to swizzle texture '%s'", filename));
        return NULL;
    }

    swizzle_fast((uint8_t*)swizzledPixels, (const uint8_t*)dataBuffer, result->pWidth * 4, result->pHeight);

    result->data = swizzledPixels;
    free(dataBuffer);

    sceKernelDcacheWritebackInvalidateAll();

    LOGINFO(stringf("Texture '%s' with size '%ld' loaded into %cram", filename, size, vram ? 'v' : ' '));

    return result;
}

void bindTexture(texture_t* tex)
{
    if (!tex)
        return;

    glTexMode(GL_PSM_8888, 0, 0, GL_TRUE);
    glTexFunc(GL_TFX_MODULATE, GL_TCC_RGBA); // output = vertex * texture
    glTexFilter(GL_NEAREST, GL_NEAREST);
    glTexWrap(GL_REPEAT, GL_REPEAT);
    glTexImage(0, tex->pWidth, tex->pHeight, tex->pWidth, tex->data);
}

void disposeTexture(texture_t* tex)
{
    if (!tex)
        return;

    free(tex);
}

void getAtlasUvIndex(textureAtlas_t* atlas, float* buffer, int index)
{
    int row = index / (int)atlas->width;
    int column = index % (int)atlas->height;

    float sizeX = 1.f / ((float)atlas->width);
    float sizeY = 1.f / ((float)atlas->height);

    float x = (float)row * sizeY;
    float y = (float)column * sizeX;

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