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

    #ifdef __PSP__
    mesh->data = memalign(16, sizeof(vertex_t) * vertexCount);
    mesh->indices = (uint16_t*)memalign(16, sizeof(uint16_t) * indexCount);
    #else
    mesh->data = malloc(sizeof(vertex_t) * vertexCount);
    mesh->indices = malloc(sizeof(uint32_t) * indexCount);
    #endif

    if (mesh->data == NULL)
        return;
    if (mesh->indices == NULL)
    {
        free(mesh->data);
        return;
    }
    memset(mesh->data, 0, sizeof(vertex_t) * vertexCount);
    memset(mesh->indices, 0, sizeof(uint16_t) * indexCount);
    mesh->indexCount = indexCount;

    #ifndef __PSP__
    memset(mesh->indices, 0, sizeof(uint32_t) * indexCount);
    glGenVertexArrays(1, &mesh->vao);
    glBindVertexArray(mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    glGenBuffers(1, &mesh->ibo);

    LOGDEBUG("VAO,VBO,IBO: %d,%d,%d (0x%16.16x,0x%16.16x,0x%16.16x)",
        mesh->vao, mesh->vbo, mesh->ibo, mesh->vao, mesh->vbo, mesh->ibo);

    mesh->vertices = vertexCount;
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices * sizeof(vertex_t), mesh->data, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(struct vertex_t, u));
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(vertex_t), (void*)offsetof(struct vertex_t, color));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(struct vertex_t, x));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t), mesh->indices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    #endif

    LOGINFO("mesh 0x%x created with %u vertices and %u indices", mesh, vertexCount, indexCount);
}

void mesh_update(mesh_t* mesh)
{
    #ifndef __PSP__
    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->vertices * sizeof(vertex_t), mesh->data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, mesh->indexCount * sizeof(uint32_t), mesh->indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    #endif
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

    if (mesh->ibo)
        glDeleteBuffers(1, &mesh->ibo);
    #endif

    free(mesh->indices);
    free(mesh->data);
}
