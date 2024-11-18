#ifndef __VERTEX_H_
#define __VERTEX_H_ 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float u, v;
    uint32_t color;
    float x, y, z;
} vertex_t;

vertex_t createVertex(float u, float v, uint32_t color, float x, float y, float z);

#ifdef __cplusplus
}
#endif

#endif