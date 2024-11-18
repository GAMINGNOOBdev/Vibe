#include <vertex.h>

vertex_t createVertex(float u, float v, uint32_t color, float x, float y, float z)
{
    vertex_t vert = {
        .u = u,
        .v = v,
        .color = color,
        .x = x,
        .y = y,
        .z = z
    };
    return vert;
}