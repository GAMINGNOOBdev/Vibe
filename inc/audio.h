#ifndef __AUDIO_H_
#define __AUDIO_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>

typedef struct
{
    float length;
    unsigned int samples;

    stb_vorbis* stream;
    stb_vorbis_info info;
} audio_stream_t;

audio_stream_t* loadAudioStream(const char* path);
void audioStreamSeekStart(audio_stream_t* astream);
void closeAudioStream(audio_stream_t* astream);

typedef void(*endAudioCallback_t)();

void initAudio();
void setAudioVolume(float volume);
void setAudioStream(audio_stream_t* astream);
void setEndAudioCallback(endAudioCallback_t callback);
void disposeAudio();

#ifdef __cplusplus
}
#endif

#endif