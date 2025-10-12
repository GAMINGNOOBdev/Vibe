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
    LOGDEBUG("vertex{ uv{%2.2f|%2.2f} color: 0x%8.8x pos{%2.2f|%2.2f|%2.2f}}",
                          vert.u, vert.v,
                          vert.color,
                          vert.x, vert.y, vert.z
                        );
}

void sprite_create(sprite_t* sprite, float x, float y, float width, float height, texture_t* texture)
{
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
    meshdata[0] = VERTEX(0, 0, 0xFFFFFFFF,  0.f,  0.f, 0.0f);
    meshdata[1] = VERTEX(0, v, 0xFFFFFFFF,  0.f,  1.f, 0.0f);
    meshdata[2] = VERTEX(u, v, 0xFFFFFFFF,  1.f,  1.f, 0.0f);
    meshdata[3] = VERTEX(u, 0, 0xFFFFFFFF,  1.f,  0.f, 0.0f);

    sprite->mesh.indices[0] = 0;
    sprite->mesh.indices[1] = 1;
    sprite->mesh.indices[2] = 2;

    sprite->mesh.indices[3] = 2;
    sprite->mesh.indices[4] = 3;
    sprite->mesh.indices[5] = 0;

    print_vertex(meshdata[0]);
    print_vertex(meshdata[1]);
    print_vertex(meshdata[2]);
    print_vertex(meshdata[3]);

    #ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll();
    #endif

    mesh_update(&sprite->mesh);

    LOGINFO("sprite 0x%x created with size (%2.2f|%2.2f) at xy (%2.2f|%2.2f) with uv (%2.2f|%2.2f)", sprite, width, height, x, y, u, v);
}

void sprite_draw(sprite_t* sprite, texture_t* texture)
{
    if (sprite == NULL || sprite->mesh.indexCount == 0)
        return;

    glMatrixMode(GL_MODEL);
    glLoadIdentity();

    #ifdef __PSP__
    ScePspFVector3 translation = {sprite->x, sprite->y, sprite->layer};
    gluTranslate(&translation);
    gluRotateZ(sprite->rotation);

    ScePspFVector3 scale = {sprite->width, sprite->height, 1.0f};
    gluScale(&scale);
    #else
    vec3 translation = GLM_VEC3_ZERO_INIT;
    translation[0] = sprite->x;
    translation[1] = sprite->y;
    vec3 scale = GLM_VEC3_ONE_INIT;
    scale[0] = sprite->width;
    scale[1] = sprite->height;
    vec3 rotation = GLM_VEC3_ZERO_INIT;
    rotation[2] = 1;

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model, translation);
    glm_scale(model, scale);
    glm_rotate(model, sprite->rotation, rotation);
    graphics_model_matrix(model);
    #endif

    texture_bind(texture);
    mesh_draw(&sprite->mesh);
}

void sprite_dispose(sprite_t* sprite)
{
    if (sprite == NULL)
        return;

    mesh_dispose(&sprite->mesh);
}
