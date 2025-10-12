#ifndef __AUDIO_H_
#define __AUDIO_H_ 1

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#define MINIMP3_NO_SIMD
#include <minimp3.h>
#include <minimp3_ex.h>
#include <wave.h>

#define AUDIO_FREQ 44100
#define AUDIO_CHANNELS 2
#define AUDIO_FRAME_SIZE 0x100
#define AUDIO_STREAMS_MAX 8
#define AUDIO_SFX_STREAMS_MAX 7
#define AUDIO_BUFFERS_PER_SOURCE 4

typedef void(*audio_end_callback_t)(void);

typedef enum
{
    AUDIO_FORMAT_INVALID,
    AUDIO_FORMAT_VORBIS,
    AUDIO_FORMAT_MP3,
    AUDIO_FORMAT_WAV,
} audio_format_t;

typedef struct
{
    float volume;

    int freq;
    float length;
    int length_ms;
    int processed_frames;
    unsigned int samples;

    stb_vorbis* stream;
    stb_vorbis_info info;
    mp3dec_ex_t mp3;
    WaveFile* wav;

    uint8_t ready;

    audio_format_t format;

    audio_end_callback_t end_callback;
} audio_stream_t;

audio_stream_t audio_stream_load(const char* path);
uint8_t audio_stream_is_valid(audio_stream_t* astream);
void audio_stream_seek_start(audio_stream_t* astream);
float audio_stream_get_position(audio_stream_t* astream);
void audio_stream_dispose(audio_stream_t* astream);

void audio_init(void);
void audio_update(void);
void audio_set_volume(float volume);
float audio_get_volume(void);
void audio_set_music_stream(audio_stream_t* astream);
void audio_play_sfx_stream(audio_stream_t* astream);
void audio_dispose(void);

#endif
