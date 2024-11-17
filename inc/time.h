#ifndef __TIME_H_
#define __TIME_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void timeInit();
void timeTick();
double timeDelta();
double timeElapsed();
uint32_t timeFPS();

#ifdef __cplusplus
}
#endif

#endif