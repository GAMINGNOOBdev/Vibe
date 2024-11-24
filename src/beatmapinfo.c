#include <beatmapinfo.h>
#include <stddef.h>
#include <memory.h>
#include <malloc.h>

beatmap_info_t* loadBeatmapInfo(const char* path)
{
    beatmap_info_t* info = malloc(sizeof(beatmap_info_t));
    if (info == NULL)
        return NULL;

    memset(info, 0, sizeof(beatmap_info_t));

    ///TODO: --- implement ---

    return info;
}

void closeBeatmapInfo(beatmap_info_t* info)
{
    if (info == NULL)
        return;

    free(info);
}