#include <mesh.h>
#include <memory.h>
#include <stdlib.h>
#include <malloc.h>
#ifdef __PSP__
#include <gu2gl.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#include <pctypes.h>
#endif
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

    #ifndef __PSP__
    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(struct vertex_t, u));
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(vertex_t), (void*)offsetof(struct vertex_t, color));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(struct vertex_t, x));

    mesh->vertices = vertexCount;
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices * sizeof(vertex_t), mesh->data, GL_DYNAMIC_DRAW);

    #endif

    LOGINFO(stringf("mesh 0x%x created with %u vertices and %u indices", mesh, vertexCount, indexCount));
}

void mesh_draw(mesh_t* mesh)
{
    if (mesh == NULL)
        return;
    
    if (mesh->indexCount == 0)
        return;

    #ifdef __PSP__
    glDrawElements(GL_TRIANGLES, GL_INDEX_16BIT | GL_TEXTURE_32BITF | GL_COLOR_8888 | GL_VERTEX_32BITF | GL_TRANSFORM_3D, mesh->indexCount, mesh->indices, mesh->data);
    #else
    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices * sizeof(vertex_t), mesh->data, GL_DYNAMIC_DRAW);
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, mesh->indices);
    glBindVertexArray(0);
    #endif
}

void mesh_dispose(mesh_t* mesh)
{
    if (mesh == NULL)
        return;

    #ifndef __PSP__
    if (mesh->vao)
        glDeleteVertexArrays(1, &mesh->vao);

    if (mesh->vbo)
        glDeleteBuffers(1, &mesh->vbo);
    #endif

    free(mesh->indices);
    free(mesh->data);
}
