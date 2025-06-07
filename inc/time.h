#ifndef __TIME_H_
#define __TIME_H_ 1

#include <stdint.h>

typedef struct
{
    double delta;
    double elapsed;
    double total;
    uint64_t total_frames;
    uint64_t currTime;
    uint64_t pastTime;
    uint64_t freq;
    uint32_t fps;
} time_data_t;

extern time_data_t time_data;

void time_init();
void time_tick();
double time_total();
uint64_t time_total_frames();
double time_delta();
double time_elapsed();
uint32_t time_fps();

#endif
