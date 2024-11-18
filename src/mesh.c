#include <mesh.h>
#include <memory.h>
#include <stdlib.h>
#include <malloc.h>
#include <gu2gl.h>

mesh_t* createMesh(uint32_t vertexCount, uint32_t indexCount)
{
    mesh_t* mesh = malloc(sizeof(mesh_t));
    if (mesh == NULL)
        return NULL;
    
    mesh->data = memalign(16, sizeof(vertex_t) * vertexCount);
    if (mesh->data == NULL)
    {
        free(mesh);
        return NULL;
    }

    mesh->indices = (uint16_t*)memalign(16, sizeof(uint16_t) * indexCount);
    if (mesh->indices == NULL)
    {
        free(mesh->data);
        free(mesh);
        return NULL;
    }

    mesh->indexCount = indexCount;

    return mesh;
}

void drawMesh(mesh_t* mesh)
{
    if (mesh == NULL)
        return;
    
    if (mesh->indexCount == 0)
        return;

    glDrawElements(GL_TRIANGLES, GL_INDEX_16BIT | GL_TEXTURE_32BITF | GL_COLOR_8888 | GL_VERTEX_32BITF | GL_TRANSFORM_3D, mesh->indexCount, mesh->indices, mesh->data);
}

void disposeMesh(mesh_t* mesh)
{
    if (mesh == NULL)
        return;

    free(mesh->indices);
    free(mesh->data);
    free(mesh);
}