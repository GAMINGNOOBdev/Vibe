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

    #ifndef __PSP__
    uint32_t id;
    #endif
} texture_t;

typedef struct
{
    float width;
    float height;
} texture_atlas_t;

texture_t* texture_load(const char* filename, const int flip, const int vram);
void texture_bind(texture_t* tex);
void texture_dispose(texture_t* tex);

void texture_atlas_get_uv_index(texture_atlas_t* atlas, float* buffer, int index);

#ifdef __cplusplus
}
#endif

#endif
