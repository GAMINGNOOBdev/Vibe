#include <pspkernel.h>
#include <logging.h>
#include <strutil.h>
#include <beatmap.h>
#include <memory.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

size_t getdelim(char **buffer, size_t *buffersz, FILE *stream, char delim) {
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    int c;

    if (buffer == NULL) {
        return -1;
    }
    if (stream == NULL) {
        return -1;
    }
    if (buffersz == NULL) {
        return -1;
    }
    bufptr = *buffer;
    size = *buffersz;

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }
    if (bufptr == NULL) {
        bufptr = malloc(128);
        if (bufptr == NULL) {
            return -1;
        }
        size = 128;
    }
    p = bufptr;
    while(c != EOF) {
        if ((p - bufptr) > (size - 1)) {
            size = size + 128;
            bufptr = realloc(bufptr, size);
            if (bufptr == NULL) {
                return -1;
            }
        }
        *p++ = c;
        if (c == delim) {
            break;
        }
        c = fgetc(stream);
    }

    *p++ = '\0';
    *buffer = bufptr;
    *buffersz = size;

    return p - bufptr - 1;
}

size_t getline(char **buffer, size_t *buffersz, FILE *stream)
{
    return getdelim(buffer, buffersz, stream, '\n');
}

beatmap_timing_point_t beatmap_timing_point_parse(const char* line)
{
    // TIMING POINT SYNTAX
    // ===================
    //
    // `time,beatLength,meter,sampleSet,sampleIndex,volume,uninherited,effects`
    //
    // time (Integer): Start time of the timing section, in milliseconds from the beginning of the beatmap's audio. The end of the timing section is the next timing point's time (or never, if this is the last timing point).
    // beatLength (Decimal): This property has two meanings:
    //     For uninherited timing points, the duration of a beat, in milliseconds.
    //     For inherited timing points, a negative inverse slider velocity multiplier, as a percentage. For example, -50 would make all sliders in this timing section twice as fast as SliderMultiplier.
    // meter (Integer): Amount of beats in a measure. Inherited timing points ignore this property.
    // sampleSet (Integer): Default sample set for hit objects (0 = beatmap default, 1 = normal, 2 = soft, 3 = drum).
    // sampleIndex (Integer): Custom sample index for hit objects. 0 indicates osu!'s default hitsounds.
    // volume (Integer): Volume percentage for hit objects.
    // uninherited (0 or 1): Whether or not the timing point is uninherited.
    // effects (Integer): Bit flags that give the timing point extra effects. See the effects section.

    beatmap_timing_point_t timing_point = {0,0,0,0,0,0,0,0};

    char* strptr = (char*)line;
    int commapos = strpos(line, ',');

    strptr[commapos] = 0;
    timing_point.time = strtoi(strptr);
    strptr[commapos] = ',';
    strptr += commapos;
    commapos = strpos(line, ',');
    strptr[commapos] = 0;
    sscanf(strptr, "%f", &timing_point.beatLength);
    strptr[commapos] = ',';
    strptr += commapos;
    commapos = strpos(line, ',');
    strptr[commapos] = 0;
    timing_point.meter = strtoi(strptr);
    strptr[commapos] = ',';
    strptr += commapos;
    commapos = strpos(line, ',');
    strptr[commapos] = 0;
    timing_point.sampleSet = strtoi(strptr);
    strptr[commapos] = ',';
    strptr += commapos;
    commapos = strpos(line, ',');
    strptr[commapos] = 0;
    timing_point.sampleIndex = strtoi(strptr);
    strptr[commapos] = ',';
    strptr += commapos;
    commapos = strpos(line, ',');
    strptr[commapos] = 0;
    timing_point.volume = strtoi(strptr);
    strptr[commapos] = ',';
    strptr += commapos;
    commapos = strpos(line, ',');
    strptr[commapos] = 0;
    timing_point.uninherited = strtoi(strptr);
    strptr[commapos] = ',';
    strptr += commapos;
    timing_point.effects = strtoi(strptr);

    return timing_point;
}

beatmap_hitobject_t beatmap_hitobject_parse(const char* line)
{
    // HITOBJECT SYNTAX
    // ================
    //
    // `x,y,time,type,hitSound,objectParams,hitSample`
    //
    // x (Integer) and y (Integer): Position in osu! pixels of the object.
    // time (Integer): Time when the object is to be hit, in milliseconds from the beginning of the beatmap's audio.
    // type (Integer): Bit flags indicating the type of the object. See the type section.
    // hitSound (Integer): Bit flags indicating the hitsound applied to the object. See the hitsound section.
    // objectParams (Comma-separated list): Extra parameters specific to the object's type.
    // hitSample (Colon-separated list): Information about which samples are played when the object is hit. It is closely related to hitSound; see the hitsounds section. If it is not written, it defaults to 0:0:0:0:.

    beatmap_hitobject_t hitobject = {0,0,0,0,0,0,0,0};

    char* strptr = (char*)line;
    int commapos = strpos(line, ',');

    strptr[commapos] = 0;
    hitobject.column = strtoi(strptr) * 4 / 512;
    strptr[commapos] = ',';
    strptr += commapos;
    commapos = strpos(line, ',');
    strptr += commapos;
    commapos = strpos(line, ',');
    strptr[commapos] = 0;
    hitobject.time = strtoi(strptr);
    strptr[commapos] = ',';
    strptr += commapos;
    commapos = strpos(line, ',');
    strptr[commapos] = 0;
    uint8_t type = strtoi(strptr);
    strptr[commapos] = ',';
    hitobject.isLN = ((type & (1 << 7)) > 0) ? 1 : 0;
    strptr += commapos;
    commapos = strpos(line, ',');
    strptr[commapos] = 0;
    hitobject.hitsound = strtoi(strptr);
    strptr[commapos] = ',';
    strptr += commapos;
    commapos = strpos(line, ':');
    strptr[commapos] = 0;
    hitobject.end = strtoi(strptr);
    strptr[commapos] = ':';

    return hitobject;
}

void beatmap_parse_general(beatmap_t* map, const char* line)
{
    if (strncmp(line, "AudioFilename: ", 15) == 0)
    {
        size_t size = strlen(line)-14;
        map->audio_path = malloc(size);
        memset(map->audio_path, 0, size);
        strncpy(map->audio_path, &line[15], size);
    }
}

void beatmap_parse_difficulty(beatmap_t* map, const char* line)
{
    if (strncmp(line, "HPDrainRate:", 12) == 0)
        sscanf(&line[12], "%f", &map->hp);
    else if (strncmp(line, "OverallDifficulty:", 18) == 0)
        sscanf(&line[18], "%f", &map->od);
    else if (strncmp(line, "ApproachRate:", 13) == 0)
        sscanf(&line[13], "%f", &map->ar);
    else if (strncmp(line, "SliderMultiplier:", 17) == 0)
        sscanf(&line[17], "%f", &map->slider_multiplier);
    else if (strncmp(line, "SliderTickRate:", 15) == 0)
        sscanf(&line[15], "%f", &map->slider_tickrate);
}

size_t timing_points_buffer_size = 0;
void beatmap_parse_timing_points(beatmap_t* map, const char* line)
{
    if (map->timing_point_count + 1 >= timing_points_buffer_size)
    {
        timing_points_buffer_size += BEATMAP_TEMP_BUFFER_SIZE;
        map->timing_points = realloc(map->timing_points, sizeof(beatmap_timing_point_t)*timing_points_buffer_size);
    }

    map->timing_points[map->timing_point_count] = beatmap_timing_point_parse(line);
    map->timing_point_count++;
}

size_t hit_objects_buffer_size = 0;
void beatmap_parse_hit_objects(beatmap_t* map, const char* line)
{
    if (map->object_count + 1 >= hit_objects_buffer_size)
    {
        hit_objects_buffer_size += BEATMAP_TEMP_BUFFER_SIZE;
        map->objects = realloc(map->objects, sizeof(beatmap_hitobject_t)*hit_objects_buffer_size);
    }

    beatmap_hitobject_t hitobject = beatmap_hitobject_parse(line);
    map->objects[map->object_count] = hitobject;

    if (hitobject.column >= 4)
        map->is_pure_4k = 0;

    map->object_count++;
}

void beatmap_parse(beatmap_t* map, const char* filepath)
{
    if (map == NULL || filepath == NULL)
        return;

    beatmap_dispose(map);
    FILE* file = fopen(filepath, "r");
    if (!file)
    {
        LOGERROR(stringf("could not open beatmap '%s': no such file", filepath));
        return;
    }

    timing_points_buffer_size = hit_objects_buffer_size = 0;

    map->is_pure_4k = 1;

    LOGDEBUG("loading following beatmap:");
    LOGDEBUG(filepath);

    beatmap_section_t section = beatmapSectionNone;

    char* line = NULL;
    size_t linesize = 0;
    while (getline(&line, &linesize, file) != EOF)
    {
        if (line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = 0;

        if (line[strlen(line)-1] == '\r')
            line[strlen(line)-1] = 0;

        if (line[0] == 0)
        {
            if (line != NULL)
                free(line);
            line = NULL;
            continue;
        }

        if (line[0] == '[')
        {
            if (strcmp(line, "[General]") == 0)
            {
                section = beatmapSectionGeneral;
                if (line != NULL)
                    free(line);
                line = NULL;
                continue;
            }
            else if (strcmp(line, "[Difficulty]") == 0)
            {
                section = beatmapSectionDifficulty;
                if (line != NULL)
                    free(line);
                line = NULL;
                continue;
            }
            else if (strcmp(line, "[TimingPoints]") == 0)
            {
                section = beatmapSectionTimingPoints;
                if (line != NULL)
                    free(line);
                line = NULL;
                continue;
            }
            else if (strcmp(line, "[HitObjects]") == 0)
            {
                section = beatmapSectionHitObjects;
                if (line != NULL)
                    free(line);
                line = NULL;
                continue;
            }
            else
            {
                section = beatmapSectionNone;
                if (line != NULL)
                    free(line);
                line = NULL;
                continue;
            }
        }

        if (section == beatmapSectionGeneral)
            beatmap_parse_general(map, line);
        else if (section == beatmapSectionDifficulty)
            beatmap_parse_difficulty(map, line);
        else if (section == beatmapSectionTimingPoints)
            beatmap_parse_timing_points(map, line);
        else if (section == beatmapSectionHitObjects)
            beatmap_parse_hit_objects(map, line);

        if (line != NULL)
            free(line);
        line = NULL;
    }
    if (line != NULL)
        free(line);

    fclose(file);

    map->timing_points = realloc(map->timing_points, sizeof(beatmap_timing_point_t)*map->timing_point_count);
    map->objects = realloc(map->objects, sizeof(beatmap_hitobject_t)*map->object_count);

    sceKernelDcacheWritebackInvalidateAll();
}

void beatmap_dispose(beatmap_t* map)
{
    if (map == NULL)
        return;

    if (map->audio_path)
        free(map->audio_path);

    if (map->timing_points != NULL)
        free(map->timing_points);

    if (map->objects > 0)
        free(map->objects);

    memset(map, 0, sizeof(beatmap_t));
}
