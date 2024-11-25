#ifndef __OPTIONS_H_
#define __OPTIONS_H_ 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t playMenuMusic;
    uint32_t audioMasterVolume;
} options_t;

void loadOptions();
void saveOptions();
options_t* getOptions();

#ifdef __cplusplus
}
#endif

#endif