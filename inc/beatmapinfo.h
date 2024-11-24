#ifndef __BEATMAPINFO_H_
#define __BEATMAPINFO_H_ 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    char* audioFileName;
    char** difficulties;
    uint8_t difficultyCount;
} beatmap_info_t;

beatmap_info_t* loadBeatmapInfo(const char* path);
void closeBeatmapInfo(beatmap_info_t* info);

#ifdef __cplusplus
}
#endif

#endif
