#include <pspkernel.h>
#include <sprite.h>
#include <vertex.h>
#include <gu2gl.h>
#include <malloc.h>

sprite_t* createSprite(float x, float y, float width, float height, texture_t* texture)
{
    sprite_t* sprite = malloc(sizeof(sprite_t));
    if (sprite == NULL)
        return NULL;

    sprite->mesh = createMesh(4, 6);
    if (sprite->mesh == NULL)
    {
        free(sprite);
        return NULL;
    }

    sprite->x = x;
    sprite->y = y;
    sprite->rotation = 0;
    sprite->layer = 0;
    sprite->width = width;
    sprite->height = height;
    sprite->texture = texture;

    ((vertex_t*)sprite->mesh->data)[0] = createVertex(0, 0, 0xFFFFFFFF, -0.25f, -0.25f, 0.0f);
    ((vertex_t*)sprite->mesh->data)[1] = createVertex(0, 1, 0xFFFFFFFF, -0.25f,  0.25f, 0.0f);
    ((vertex_t*)sprite->mesh->data)[2] = createVertex(1, 1, 0xFFFFFFFF,  0.25f,  0.25f, 0.0f);
    ((vertex_t*)sprite->mesh->data)[3] = createVertex(1, 0, 0xFFFFFFFF,  0.25f, -0.25f, 0.0f);

    sprite->mesh->indices[0] = 0;
    sprite->mesh->indices[1] = 1;
    sprite->mesh->indices[2] = 2;

    sprite->mesh->indices[3] = 2;
    sprite->mesh->indices[4] = 3;
    sprite->mesh->indices[5] = 0;

    sceKernelDcacheWritebackInvalidateAll();

    return sprite;
}

void drawSprite(sprite_t* sprite)
{
    if (sprite == NULL) return;
    if (sprite->texture == NULL || sprite->mesh == NULL) return;
    if (sprite->mesh->indexCount == 0) return;

    glMatrixMode(GL_MODEL);
    glLoadIdentity();

    ScePspFVector3 translation = {sprite->x, sprite->y, sprite->layer};
    gluTranslate(&translation);
    gluRotateZ(sprite->rotation);

    ScePspFVector3 scale = {sprite->width, sprite->height, 1.0f};
    gluScale(&scale);

    bindTexture(sprite->texture);
    drawMesh(sprite->mesh);
}

void disposeSprite(sprite_t* sprite)
{
    if (sprite == NULL) return;

    disposeMesh(sprite->mesh);
    free(sprite);
}