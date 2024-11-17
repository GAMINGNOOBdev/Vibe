#include <audio.h>
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <pspaudiolib.h>
#include <pspaudio.h>
#include <logging.h>
#include <malloc.h>

audio_stream* loadAudioStream(const char* path)
{
    if (!path)
        return NULL;
    
    audio_stream* astream = malloc(sizeof(audio_stream));
    if (!astream)
        return NULL;

    astream->stream = stb_vorbis_open_filename(path, NULL, NULL);
    astream->info = stb_vorbis_get_info(astream->stream);
    astream->length = stb_vorbis_stream_length_in_seconds(astream->stream);
    astream->samples = stb_vorbis_stream_length_in_samples(astream->stream);

    LOGINFO(stringf("sample rate: '%d' | channels: '%d' | max_frame_size: '%d' | length: '%f' | samples: '%d'", astream->info.sample_rate, astream->info.channels, astream->info.max_frame_size, astream->length, astream->samples));

    return astream;
}

void audioStreamSeekStart(audio_stream* astream)
{
    if (!astream)
        return;

    stb_vorbis_seek_start(astream->stream);
}

void closeAudioStream(audio_stream* astream)
{
    if (!astream)
        return;

    stb_vorbis_close(astream->stream);
    free(astream);
}

endAudioCallback_t audioEndCallback = NULL;
audio_stream* global_audio_stream = NULL;

void audioCallback(void* buffer, unsigned int length, void* userdata)
{
    audio_stream* astream = global_audio_stream;
    if (!astream)
        return;

    short* buf = (short*)buffer;
    int numFrames = length*astream->info.channels;
    int frames = stb_vorbis_get_samples_short_interleaved(astream->stream, astream->info.channels, buf, numFrames);

    if (frames < length && audioEndCallback != NULL)
        audioEndCallback();
}

void initAudio()
{
    pspAudioInit();
    pspAudioSetChannelCallback(0, audioCallback, NULL);
}

void setAudioVolume(float volume)
{
    int vol = (int)(volume * (float)PSP_VOLUME_MAX);
    pspAudioSetVolume(0, vol, vol);
}

void setAudioStream(audio_stream* astream)
{
    global_audio_stream = astream;
}

void setEndAudioCallback(endAudioCallback_t callback)
{
    audioEndCallback = callback;
}

void disposeAudio()
{
    pspAudioEnd();
}