#ifndef __VERTEX_H_
#define __VERTEX_H_ 1

#include <stdint.h>

#ifdef _WIN32
#define PACKED
#else
#define PACKED __attribute__((packed))
#endif
typedef struct vertex_t
{
    float u;
    float v;
    uint32_t color;
    float x;
    float y;
    float z;
} PACKED vertex_t;

#define VERTEX(_u,_v,_color,_x,_y,_z) (vertex_t){.u=_u,.v=_v,.color=_color,.x=_x,.y=_y,.z=_z}

#endif
