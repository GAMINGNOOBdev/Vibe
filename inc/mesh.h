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
} mesh_t;

mesh_t* createMesh(uint32_t vertexCount, uint32_t indexCount);
void drawMesh(mesh_t* mesh);
void disposeMesh(mesh_t* mesh);

#ifdef __cplusplus
}
#endif

#endif