#ifndef __MESH_H_
#define __MESH_H_ 1

#include <stdint.h>
#include <vertex.h>

typedef struct
{
    void* data;
    uint32_t indexCount;
    
    #ifdef __PSP__
    uint16_t* indices;
    #else
    uint32_t* indices;
    uint32_t vertices;
    uint32_t vao, vbo, ibo;
    #endif
} mesh_t;

void mesh_create(mesh_t* mesh, uint32_t vertexCount, uint32_t indexCount);
void mesh_update(mesh_t* mesh);
void mesh_draw(mesh_t* mesh);
void mesh_dispose(mesh_t* mesh);

#endif
