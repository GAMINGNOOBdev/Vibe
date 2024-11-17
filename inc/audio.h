#ifndef __AUDIO_H_
#define __AUDIO_H_ 1

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>

typedef struct audio_stream
{
    float length;
    unsigned int samples;

    stb_vorbis* stream;
    stb_vorbis_info info;
} audio_stream;

audio_stream* loadAudioStream(const char* path);
void audioStreamSeekStart(audio_stream* astream);
void closeAudioStream(audio_stream* astream);

typedef void(*endAudioCallback_t)();

void initAudio();
void setAudioVolume(float volume);
void setAudioStream(audio_stream* astream);
void setEndAudioCallback(endAudioCallback_t callback);
void disposeAudio();

#endif