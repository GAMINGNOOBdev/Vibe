#ifndef __PCTYPES_H_
#define __PCTYPES_H_ 1

#ifndef __PSP__

#include <SDL2/SDL.h>

typedef struct
{
    float x, y, z;
} ScePspFVector3;

#define gluTranslate(translation) glTranslatef((translation)->x, (translation)->y, (translation)->z);
#define gluRotateZ(rot) glRotatef(rot, 0, 0, 1);
#define gluScale(scale) glScalef((scale)->x, (scale)->y, (scale)->z)
#define GL_MODEL GL_MODELVIEW

#define PSP_CTRL_MODE_ANALOG 0
#define PSP_CTRL_START SDLK_SPACE
#define PSP_CTRL_SELECT SDLK_RETURN
#define PSP_CTRL_CROSS SDLK_y
#define PSP_CTRL_CIRCLE SDLK_x
#define PSP_CTRL_UP SDLK_UP
#define PSP_CTRL_DOWN SDLK_DOWN
#define PSP_CTRL_LEFT SDLK_LEFT
#define PSP_CTRL_RIGHT SDLK_RIGHT
#define PSP_CTRL_LTRIGGER SDLK_q
#define PSP_CTRL_RTRIGGER SDLK_e

#endif // __PSP__

#endif // __PCTYPES_H_
