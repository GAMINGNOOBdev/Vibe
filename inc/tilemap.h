#ifndef __TILEMAP_H_
#define __TILEMAP_H_ 1

#include <texture.h>
#include <stdint.h>
#include <mesh.h>

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
    texture_atlas_t atlas;
    texture_t* texture;
    mesh_t mesh;
} tilemap_t;

void tilemap_create(tilemap_t* tilemap, texture_atlas_t atlas, texture_t* texture, int sizex, int sizey);
void tilemap_build(tilemap_t* tilemap);
void tilemap_draw(tilemap_t* tilemap);
void tilemap_dispose(tilemap_t* tilemap);

#endif
