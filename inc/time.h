#ifndef __TIME_H_
#define __TIME_H_ 1

#include <stdint.h>

typedef struct
{
    double delta;
    double elapsed;
    uint64_t currTime;
    uint64_t pastTime;
    uint32_t freq;
    uint32_t fps;
} time_data_t;

extern time_data_t time_data;

void time_init();
void time_tick();
double time_delta();
double time_elapsed();
uint32_t time_fps();

#endif