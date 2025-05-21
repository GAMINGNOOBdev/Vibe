#ifdef __PSP__
#include <pspkernel.h>
#endif

#include <logging.h>
#include <sprite.h>
#include <vertex.h>
#ifdef __PSP__
#include <gu2gl.h>
#else
#include <GL/glew.h>
#include <gfx.h>
#include <pctypes.h>
#endif
#include <malloc.h>

void print_vertex(vertex_t vert)
{
    LOGDEBUG(stringf("vertex{ uv{%2.2f|%2.2f} color: 0x%8.8x pos{%2.2f|%2.2f|%2.2f}}",
                          vert.u, vert.v,
                          vert.color,
                          vert.x, vert.y, vert.z
                        ));
}

sprite_t* sprite_create(float x, float y, float width, float height, texture_t* texture)
{
    sprite_t* sprite = (sprite_t*)malloc(sizeof(sprite_t));
    if (sprite == NULL)
        return NULL;

    float u = 1;
    float v = 1;

    if (texture != NULL)
    {
        u = (float)texture->width / (float)texture->pWidth;
        v = (float)texture->height / (float)texture->pHeight;
    }

    mesh_create(&sprite->mesh, 4, 6);
    sprite->x = x;
    sprite->y = y;
    sprite->rotation = 0;
    sprite->layer = 0;
    sprite->width = width;
    sprite->height = height;

    vertex_t* meshdata = (vertex_t*)sprite->mesh.data;
    uint16_t* meshindices = sprite->mesh.indices;
    meshdata[0] = VERTEX(0, 0, 0xFFFFFFFF,  0.f,  0.f, 0.0f);
    meshdata[1] = VERTEX(0, v, 0xFFFFFFFF,  0.f,  1.f, 0.0f);
    meshdata[2] = VERTEX(u, v, 0xFFFFFFFF,  1.f,  1.f, 0.0f);
    meshdata[3] = VERTEX(u, 0, 0xFFFFFFFF,  1.f,  0.f, 0.0f);

    meshindices[0] = 0;
    meshindices[1] = 1;
    meshindices[2] = 2;

    meshindices[3] = 2;
    meshindices[4] = 3;
    meshindices[5] = 0;

    print_vertex(meshdata[0]);
    print_vertex(meshdata[1]);
    print_vertex(meshdata[2]);
    print_vertex(meshdata[3]);

    #ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll();
    #endif

    LOGINFO(stringf("sprite 0x%x created with size (%2.2f|%2.2f) at xy (%2.2f|%2.2f) with uv (%2.2f|%2.2f)", sprite, width, height, x, y, u, v));

    return sprite;
}

void sprite_draw(sprite_t* sprite, texture_t* texture)
{
    if (sprite == NULL || texture == NULL || sprite->mesh.indexCount == 0)
        return;

    glMatrixMode(GL_MODEL);
    glLoadIdentity();

    ScePspFVector3 translation = {sprite->x, sprite->y, sprite->layer};
    gluTranslate(&translation);
    gluRotateZ(sprite->rotation);

    ScePspFVector3 scale = {sprite->width, sprite->height, 1.0f};
    gluScale(&scale);

    #ifndef __PSP__
    graphics_projection_matrix();
    #endif

    texture_bind(texture);
    mesh_draw(&sprite->mesh);
}

void sprite_dispose(sprite_t* sprite)
{
    if (sprite == NULL)
        return;

    mesh_dispose(&sprite->mesh);
    free(sprite);
}
