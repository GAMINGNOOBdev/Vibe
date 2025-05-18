#ifndef __TIME_H_
#define __TIME_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void time_init();
void time_tick();
double time_delta();
double time_elapsed();
uint32_t time_fps();

#ifdef __cplusplus
}
#endif

#endif