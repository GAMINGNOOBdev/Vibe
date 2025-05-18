#ifndef __AUDIO_H_
#define __AUDIO_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#define MINIMP3_NO_SIMD
#include <minimp3.h>
#include <minimp3_ex.h>
#include <wave.h>

typedef enum
{
    AUDIO_FORMAT_VORBIS,
    AUDIO_FORMAT_MP3,
    AUDIO_FORMAT_WAV,
} audio_format_t;

typedef struct
{
    float length;
    unsigned int samples;

    stb_vorbis* stream;
    stb_vorbis_info info;

    mp3dec_ex_t mp3;

    WaveFile* wav;

    audio_format_t format;
} audio_stream_t;

void audio_stream_load(audio_stream_t* stream, const char* path);
void audio_stream_seek_start(audio_stream_t* astream);
void audio_stream_dispose(audio_stream_t* astream);

typedef void(*audio_end_callback_t)(void);

void audio_init(void);
void audio_set_volume(float volume);
float audio_get_volume(void);
void audio_set_stream(audio_stream_t* astream);
void audio_set_end_callback(audio_end_callback_t callback);
void audio_dispose(void);

#ifdef __cplusplus
}
#endif

#endif
