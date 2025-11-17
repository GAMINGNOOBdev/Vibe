#include <stdint.h>
#ifndef __AUDIO_H_
#define __AUDIO_H_ 1

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <minimp3.h>
#include <minimp3_ex.h>
#include <wave.h>

#define AUDIO_FREQ 44100
#define AUDIO_CHANNELS 2
#ifdef __PSP__
#define AUDIO_FRAME_SIZE 0x100
#else
#define AUDIO_FRAME_SIZE 0x400
#endif
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

    uint32_t freq;
    float length;
    int length_ms;
    float seconds; // current position in seconds
    uint8_t playing;
    uint32_t samples;
    int processed_frames;

    stb_vorbis* stream;
    stb_vorbis_info info;
    mp3dec_ex_t mp3;
    WaveFile* wav;

    audio_format_t format;
    audio_end_callback_t end_callback;
} audio_stream_t;

audio_stream_t audio_stream_load(const char* path);
uint8_t audio_stream_is_valid(audio_stream_t* astream);
void audio_stream_update(audio_stream_t* astream, int index);
void audio_stream_update_buffer(audio_stream_t* astream, void* buffer, int bytes);
void audio_stream_seek_start(audio_stream_t* astream);
void audio_stream_dispose(audio_stream_t* astream);

void audio_init(void);
void audio_update(void);
void audio_set_volume(float volume);
float audio_get_volume(void);
void audio_set_music_stream(audio_stream_t* astream);
void audio_play_sfx_stream(audio_stream_t* astream);
void audio_dispose(void);

#endif
