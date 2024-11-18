#ifndef __TEXTURE_H_
#define __TEXTURE_H_ 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint32_t width, height;
    uint32_t pWidth, pHeight;
    void* data;
} texture_t;

typedef struct
{
    float width;
    float height;
} textureAtlas_t;

texture_t* loadTexture(const char* filename, const int flip, const int vram);
void bindTexture(texture_t* tex);
void disposeTexture(texture_t* tex);

void getAtlasUvIndex(textureAtlas_t* atlas, float* buffer, int index);

#ifdef __cplusplus
}
#endif

#endif