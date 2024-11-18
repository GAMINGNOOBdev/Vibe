#include <pspkernel.h>
#include <tilemap.h>
#include <memory.h>
#include <malloc.h>
#include <gu2gl.h>

tilemap_t* createTilemap(textureAtlas_t* atlas, texture_t* texture, int sizex, int sizey)
{
    tilemap_t* tilemap = malloc(sizeof(tilemap_t));
    if (tilemap == NULL)
        return NULL;
    
    tilemap->tiles = (tile_t*)malloc(sizeof(tile_t)*sizex*sizey);
    if (tilemap->tiles == NULL)
    {
        free(tilemap);
        return NULL;
    }

    tilemap->mesh = createMesh(sizex*sizey*4, sizex*sizey*6);
    if (tilemap->mesh == NULL)
    {
        free(tilemap->tiles);
        tilemap->tiles = NULL;
        return NULL;
    }

    memset(tilemap->tiles, 0, sizeof(tile_t)*sizex*sizey);

    tilemap->atlas = atlas;
    tilemap->texture = texture;
    tilemap->x = 0;
    tilemap->y = 0;
    tilemap->width = sizex;
    tilemap->height = sizey;
    tilemap->scalex = 16.0f;
    tilemap->scaley = 16.0f;

    return tilemap;
}

void buildTilemap(tilemap_t* tilemap)
{
    if (tilemap == NULL)
        return;
    if (tilemap->atlas == NULL)
        return;

    size_t size = tilemap->width * tilemap->height;
    for (int i = 0; i < size; i++)
    {
        float buffer[8];
        getAtlasUvIndex(tilemap->atlas, buffer, tilemap->tiles[i].idx);

        float tx = (float)tilemap->tiles[i].x;
        float ty = (float)tilemap->tiles[i].y;
        float tw = tx + 1.0f;
        float th = ty + 1.0f;

        ((vertex_t*)tilemap->mesh->data)[i * 4 + 0] = createVertex(buffer[0], buffer[1], 0xFFFFFFFF, tx, ty, 0.0f);
        ((vertex_t*)tilemap->mesh->data)[i * 4 + 1] = createVertex(buffer[2], buffer[3], 0xFFFFFFFF, tx, th, 0.0f);
        ((vertex_t*)tilemap->mesh->data)[i * 4 + 2] = createVertex(buffer[4], buffer[5], 0xFFFFFFFF, tw, th, 0.0f);
        ((vertex_t*)tilemap->mesh->data)[i * 4 + 3] = createVertex(buffer[6], buffer[7], 0xFFFFFFFF, tw, ty, 0.0f);

        tilemap->mesh->indices[i * 6 + 0] = (i * 4) + 0;
        tilemap->mesh->indices[i * 6 + 1] = (i * 4) + 1;
        tilemap->mesh->indices[i * 6 + 2] = (i * 4) + 2;
        tilemap->mesh->indices[i * 6 + 3] = (i * 4) + 2;
        tilemap->mesh->indices[i * 6 + 4] = (i * 4) + 3;
        tilemap->mesh->indices[i * 6 + 5] = (i * 4) + 0;
    }

    sceKernelDcacheWritebackInvalidateAll();
}

void drawTilemap(tilemap_t* tilemap)
{
    if (tilemap == NULL)
        return;
    if (tilemap->mesh->indexCount == 0 || tilemap->texture == NULL || tilemap->atlas == NULL)
        return;

    glMatrixMode(GL_MODEL);
    glLoadIdentity();

    ScePspFVector3 translation = {tilemap->x, tilemap->y, 0};
    gluTranslate(&translation);

    ScePspFVector3 scale = {tilemap->scalex, tilemap->scaley, 0};
    gluScale(&scale);

    bindTexture(tilemap->texture);
    drawMesh(tilemap->mesh);
}

void disposeTilemap(tilemap_t* tilemap)
{
    if (tilemap == NULL)
        return;

    disposeMesh(tilemap->mesh);
    free(tilemap->tiles);
    free(tilemap);
}