#include <audio.h>
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#define MINIMP3_IMPLEMENTATION
#include <minimp3.h>
#include <minimp3_ex.h>
#include <pspaudiolib.h>
#include <pspkernel.h>
#include <pspaudio.h>
#include <logging.h>
#include <strutil.h>
#include <malloc.h>
#include <memory.h>

////////////////////////
///                  ///
/// Global variables ///
///                  ///
////////////////////////

float audioVolume = 1.0f;
endAudioCallback_t audioEndCallback = NULL;
audio_stream_t* global_audio_stream = NULL;

////////////////////////
///                  ///
/// Global functions ///
///                  ///
////////////////////////

audio_stream_t* loadAudioStream(const char* path)
{
    if (!path)
        return NULL;

    audio_stream_t* astream = malloc(sizeof(audio_stream_t));
    if (!astream)
        return NULL;

    int lastDot = strlpos(path, '.');
    int vorbis = strcmp(&path[lastDot+1], "ogg");
    int mp3 = strcmp(&path[lastDot+1], "mp3");
    int wav = strcmp(&path[lastDot+1], "wav");

    if (vorbis == 0)
    {
        LOGINFO("loading ogg vorbis file");

        astream->stream = stb_vorbis_open_filename(path, NULL, NULL);
        astream->info = stb_vorbis_get_info(astream->stream);
        astream->length = stb_vorbis_stream_length_in_seconds(astream->stream);
        astream->samples = stb_vorbis_stream_length_in_samples(astream->stream);

        LOGINFO(stringf("sample rate: '%d' | channels: '%d' | max_frame_size: '%d' | length: '%f' | samples: '%d'", astream->info.sample_rate, astream->info.channels, astream->info.max_frame_size, astream->length, astream->samples));

        astream->format = AUDIO_FORMAT_VORBIS;
    }
    else if (mp3 == 0)
    {
        LOGINFO("loading mp3 file");

        if (mp3dec_ex_open(&astream->mp3, path, MP3D_SEEK_TO_BYTE))
        {
            free(astream);
            LOGERROR(stringf("Could not load audio file '%s': Unknown error"));
            return NULL;
        }

        astream->samples = astream->mp3.samples;
        astream->length = (float)astream->mp3.samples / (float)astream->mp3.info.hz;

        LOGINFO(stringf("sample rate: '%d' | channels: '%d' | frame_bytes: '%d' | length: '%f' | samples: '%d'", astream->mp3.info.hz, astream->mp3.info.channels, astream->mp3.info.frame_bytes, astream->length, astream->samples));

        astream->format = AUDIO_FORMAT_MP3;
    }
    else if (wav == 0)
    {
        LOGINFO("loading wav file");

        astream->wav = wave_open(path, WAVE_OPEN_READ);
        if (wave_get_format(astream->wav) != WAVE_FORMAT_PCM)
        {
            LOGERROR("could not load wav file: expected int16 format");
            wave_close(astream->wav);
            free(astream);
            return NULL;
        }

        astream->format = AUDIO_FORMAT_WAV;

        LOGINFO(stringf("sample rate: '%ld' | channels: '%d' | length: '%ld'", wave_get_sample_rate(astream->wav), wave_get_num_channels(astream->wav), wave_get_length(astream->wav)));
    }
    else
    {
        free(astream);
        LOGERROR(stringf("Could not load audio file '%s': Invalid format"));
        return NULL;
    }

    return astream;
}

void audioStreamSeekStart(audio_stream_t* astream)
{
    if (!astream)
        return;

    if (astream->format == AUDIO_FORMAT_VORBIS)
    {
        stb_vorbis_seek_start(astream->stream);
        LOGDEBUG("vorbis stream seeked to start");
        return;
    }
    else if (astream->format == AUDIO_FORMAT_MP3)
    {
        mp3dec_ex_seek(&astream->mp3, 0);
        LOGDEBUG("mp3 stream seeked to start");
        return;
    }

    wave_seek(astream->wav, 0, 0);
    LOGDEBUG("wav stream seeked to start");
}

void closeAudioStream(audio_stream_t* astream)
{
    if (!astream)
        return;

    if (astream->format == AUDIO_FORMAT_VORBIS)
        stb_vorbis_close(astream->stream);
    else if (astream->format == AUDIO_FORMAT_MP3)
        mp3dec_ex_close(&astream->mp3);
    else
        wave_close(astream->wav);

    free(astream);
}

void audioCallback(void* buffer, unsigned int length, void* userdata)
{
    audio_stream_t* astream = global_audio_stream;
    if (!astream)
    {
        memset(buffer, 0, sizeof(short) * length * 2);
        return;
    }

    int frames = 0;
    int numFrames = length;

    if (astream->format == AUDIO_FORMAT_VORBIS)
    {
        short* buf = (short*)buffer;
        numFrames *= astream->info.channels;
        frames = stb_vorbis_get_samples_short_interleaved(astream->stream, astream->info.channels, buf, numFrames);
    }
    else if (astream->format == AUDIO_FORMAT_MP3)
    {
        numFrames *= astream->mp3.info.channels;
        mp3dec_frame_info_t frameInfo;
        frames = mp3dec_ex_read(&astream->mp3, (mp3d_sample_t*)buffer, numFrames) / astream->mp3.info.channels;
    }
    else
    {
        frames = wave_read(astream->wav, buffer, length);
        /*float* samples = malloc(sizeof(float)*length*2);
        frames = tinywav_read_f(&astream->wav, samples, length);
        short* buf = buffer;
        for (int i = 0; i < length; i++)
        {
            buf[i*2 + 0] = samples[i*2 + 0];
            buf[i*2 + 1] = samples[i*2 + 1];
        }
        free(samples);
        sceKernelDcacheWritebackInvalidateAll();*/
    }

end:

    if (frames < length && audioEndCallback != NULL)
        audioEndCallback();
}

void initAudio()
{
    pspAudioInit();
    pspAudioSetChannelCallback(0, audioCallback, NULL);
    setAudioVolume(1.0f);
}

void setAudioVolume(float volume)
{
    if (volume < 0)
        volume = 0;
    
    if (volume > 1)
        volume = 1;

    int vol = (int)(volume * (float)PSP_VOLUME_MAX);
    pspAudioSetVolume(0, vol, vol);
    audioVolume = volume;
}

float getAudioVolume()
{
    return audioVolume;
}

void setAudioStream(audio_stream_t* astream)
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