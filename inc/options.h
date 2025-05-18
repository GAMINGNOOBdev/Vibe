#ifndef __OPTIONS_H_
#define __OPTIONS_H_ 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float master_volume;
    float music_volume;
    float hitsound_volume;
} options_t;

extern options_t options;

void options_load(void);
void options_save(void);

#ifdef __cplusplus
}
#endif

#endif