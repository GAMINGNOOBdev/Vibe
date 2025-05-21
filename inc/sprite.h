#ifndef __SPRITE_H_
#define __SPRITE_H_ 1

#include <texture.h>
#include <mesh.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float x, y;
    float rotation;
    float width, height;
    int layer;
    mesh_t mesh;
} sprite_t;

sprite_t* sprite_create(float x, float y, float width, float height, texture_t* texture);
void sprite_draw(sprite_t* sprite, texture_t* texture);
void sprite_dispose(sprite_t* sprite);

#ifdef __cplusplus
}
#endif

#endif
