#ifndef __TILEMAP_H_
#define __TILEMAP_H_ 1

#include <texture.h>
#include <stdint.h>
#include <mesh.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int x, y, idx;
    uint32_t color;
} tile_t;

typedef struct
{
    float x, y;
    float scalex, scaley;
    int width, height;
    tile_t* tiles;
    textureAtlas_t* atlas;
    texture_t* texture;
    mesh_t* mesh;
} tilemap_t;

tilemap_t* createTilemap(textureAtlas_t* atlas, texture_t* texture, int sizex, int sizey);
void buildTilemap(tilemap_t* tilemap);
void drawTilemap(tilemap_t* tilemap);
void disposeTilemap(tilemap_t* tilemap);

#ifdef __cplusplus
}
#endif

#endif