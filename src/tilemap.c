#ifdef __PSP__
#include <pspkernel.h>
#endif

#include <tilemap.h>
#include <texture.h>
#include <memory.h>
#include <malloc.h>
#ifdef __PSP__
#include <gu2gl.h>
#else
#include <GL/glew.h>
#include <gfx.h>
#include <pctypes.h>
#endif
#include <mesh.h>

void tilemap_create(tilemap_t* tilemap, texture_atlas_t atlas, texture_t* texture, int sizex, int sizey)
{
    tilemap_dispose(tilemap);
    tilemap->tiles = (tile_t*)malloc(sizeof(tile_t)*sizex*sizey);
    if (tilemap->tiles == NULL)
    {
        free(tilemap);
        return;
    }

    mesh_create(&tilemap->mesh, sizex*sizey*4, sizex*sizey*6);

    tilemap->atlas = atlas;
    tilemap->texture = texture;
    tilemap->x = 0;
    tilemap->y = 0;
    tilemap->width = sizex;
    tilemap->height = sizey;
    tilemap->scalex = 16.0f;
    tilemap->scaley = 16.0f;
}

void tilemap_build(tilemap_t* tilemap)
{
    if (tilemap == NULL)
        return;

    size_t size = tilemap->width * tilemap->height;
    for (int i = 0; i < size; i++)
    {
        float buffer[8];
        texture_atlas_get_uv_index(&tilemap->atlas, buffer, tilemap->tiles[i].idx);

        float tx = (float)tilemap->tiles[i].x;
        float ty = (float)tilemap->tiles[i].y;
        float tw = tx + 1.0f;
        float th = ty + 1.0f;

        ((vertex_t*)tilemap->mesh.data)[i * 4 + 0] = VERTEX(buffer[0], buffer[1], tilemap->tiles[i].color, tx, ty, 0.0f);
        ((vertex_t*)tilemap->mesh.data)[i * 4 + 1] = VERTEX(buffer[2], buffer[3], tilemap->tiles[i].color, tx, th, 0.0f);
        ((vertex_t*)tilemap->mesh.data)[i * 4 + 2] = VERTEX(buffer[4], buffer[5], tilemap->tiles[i].color, tw, th, 0.0f);
        ((vertex_t*)tilemap->mesh.data)[i * 4 + 3] = VERTEX(buffer[6], buffer[7], tilemap->tiles[i].color, tw, ty, 0.0f);

        tilemap->mesh.indices[i * 6 + 0] = (i * 4) + 0;
        tilemap->mesh.indices[i * 6 + 1] = (i * 4) + 1;
        tilemap->mesh.indices[i * 6 + 2] = (i * 4) + 2;
        tilemap->mesh.indices[i * 6 + 3] = (i * 4) + 2;
        tilemap->mesh.indices[i * 6 + 4] = (i * 4) + 3;
        tilemap->mesh.indices[i * 6 + 5] = (i * 4) + 0;
    }

    #ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll();
    #endif
}

void tilemap_draw(tilemap_t* tilemap)
{
    if (tilemap == NULL)
        return;
    if (tilemap->mesh.indexCount == 0 || tilemap->texture == NULL)
        return;

    glMatrixMode(GL_MODEL);
    glLoadIdentity();

    ScePspFVector3 translation = {tilemap->x, tilemap->y, 0};
    gluTranslate(&translation);

    ScePspFVector3 scale = {tilemap->scalex, tilemap->scaley, 0};
    gluScale(&scale);

    #ifndef __PSP__
    graphics_projection_matrix();
    #endif

    texture_bind(tilemap->texture);
    mesh_draw(&tilemap->mesh);
}

void tilemap_dispose(tilemap_t* tilemap)
{
    if (tilemap == NULL)
        return;

    mesh_dispose(&tilemap->mesh);
    free(tilemap->tiles);
    memset(tilemap, 0, sizeof(tilemap_t));
}