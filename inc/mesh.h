#ifndef __MESH_H_
#define __MESH_H_ 1

#include <stdint.h>
#include <vertex.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void* data;
    uint16_t* indices;
    uint32_t indexCount;

    #ifndef __PSP__
    uint32_t vertices;
    uint32_t vao, vbo;
    #endif
} mesh_t;

void mesh_create(mesh_t* mesh, uint32_t vertexCount, uint32_t indexCount);
void mesh_draw(mesh_t* mesh);
void mesh_dispose(mesh_t* mesh);

#ifdef __cplusplus
}
#endif

#endif
