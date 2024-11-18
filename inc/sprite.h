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
    mesh_t* mesh;
    texture_t* texture;
} sprite_t;

sprite_t* createSprite(float x, float y, float width, float height, texture_t* texture);
void drawSprite(sprite_t* sprite);
void disposeSprite(sprite_t* sprite);

#ifdef __cplusplus
}
#endif

#endif