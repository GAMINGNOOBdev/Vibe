#include <mesh.h>
#include <memory.h>
#include <stdlib.h>
#include <malloc.h>
#include <gu2gl.h>
#include <logging.h>

void mesh_create(mesh_t* mesh, uint32_t vertexCount, uint32_t indexCount)
{
    if (mesh == NULL)
        return;

    mesh->data = memalign(16, sizeof(vertex_t) * vertexCount);
    memset(mesh->data, 0, sizeof(vertex_t) * vertexCount);
    if (mesh->data == NULL)
        return;

    mesh->indices = (uint16_t*)memalign(16, sizeof(uint16_t) * indexCount);
    memset(mesh->indices, 0, sizeof(uint16_t) * indexCount);
    if (mesh->indices == NULL)
    {
        free(mesh->data);
        return;
    }

    mesh->indexCount = indexCount;

    LOGINFO(stringf("mesh 0x%x created with %u vertices and %u indices", mesh, vertexCount, indexCount));
}

void mesh_draw(mesh_t* mesh)
{
    if (mesh == NULL)
        return;
    
    if (mesh->indexCount == 0)
        return;

    glDrawElements(GL_TRIANGLES, GL_INDEX_16BIT | GL_TEXTURE_32BITF | GL_COLOR_8888 | GL_VERTEX_32BITF | GL_TRANSFORM_3D, mesh->indexCount, mesh->indices, mesh->data);
}

void mesh_dispose(mesh_t* mesh)
{
    if (mesh == NULL)
        return;

    free(mesh->indices);
    free(mesh->data);
}
