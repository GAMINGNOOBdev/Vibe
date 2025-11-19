#ifndef __BEATMAP_H_
#define __BEATMAP_H_ 1

#include <stddef.h>
#include <stdint.h>

#define BEATMAP_TEMP_BUFFER_SIZE 0x80

typedef struct
{
    int time;
    float beatLength;
    int meter;
    int sampleSet;
    int sampleIndex;
    int volume;
    int uninherited;
    int effects;
} beatmap_timing_point_t;

typedef struct
{
    int time;
    int end;

    uint8_t hitsound;

    uint8_t column : 3;
    uint8_t hit : 1;
    uint8_t held : 1;
    uint8_t isLN : 1;
    uint8_t tailHit : 1;
    uint8_t reserved : 1;
} beatmap_hitobject_t;

typedef enum
{
    beatmapSectionNone,
    beatmapSectionGeneral,
    beatmapSectionMetadata,
    beatmapSectionDifficulty,
    beatmapSectionTimingPoints,
    beatmapSectionHitObjects,
} beatmap_section_t;

typedef struct
{
    float ar, od, hp;
    float slider_tickrate;
    float slider_multiplier;
    uint64_t set_id;
    uint64_t id;

    uint8_t is_pure_4k;

    size_t hit_count;
    size_t object_count;
    size_t timing_point_count;

    char* audio_path;
    beatmap_hitobject_t* objects;
    beatmap_timing_point_t* timing_points;
} beatmap_t;

void beatmap_parse(beatmap_t* map, const char* filepath);
void beatmap_dispose(beatmap_t* map);
float beatmap_calculate_sv(beatmap_timing_point_t* timings, int timing_count, beatmap_hitobject_t hitobject);

#endif
