#ifndef __SPRITE_H_
#define __SPRITE_H_ 1

#include <texture.h>
#include <mesh.h>

typedef struct
{
    float x, y;
    float rotation;
    float width, height;
    int layer;
    mesh_t mesh;
} sprite_t;

void sprite_create(sprite_t* sprite, float x, float y, float width, float height, texture_t* texture);
void sprite_draw(sprite_t* sprite, texture_t* texture);
void sprite_dispose(sprite_t* sprite);

#endif
