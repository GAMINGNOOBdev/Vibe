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
audio_end_callback_t audioEndCallback = NULL;
audio_stream_t* global_audio_stream = NULL;

////////////////////////
///                  ///
/// Global functions ///
///                  ///
////////////////////////

void audio_stream_load(audio_stream_t* stream, const char* path)
{
    if (!path || !stream)
        return;

    int lastDot = strlpos(path, '.');
    int vorbis = strcmp(&path[lastDot+1], "ogg");
    int mp3 = strcmp(&path[lastDot+1], "mp3");
    int wav = strcmp(&path[lastDot+1], "wav");

    if (vorbis == 0)
    {
        LOGINFO("loading ogg vorbis file");

        stream->stream = stb_vorbis_open_filename(path, NULL, NULL);
        stream->info = stb_vorbis_get_info(stream->stream);
        stream->length = stb_vorbis_stream_length_in_seconds(stream->stream);
        stream->samples = stb_vorbis_stream_length_in_samples(stream->stream);

        LOGINFO(stringf("sample rate: '%d' | channels: '%d' | max_frame_size: '%d' | length: '%f' | samples: '%d'", stream->info.sample_rate, stream->info.channels, stream->info.max_frame_size, stream->length, stream->samples));

        stream->format = AUDIO_FORMAT_VORBIS;
    }
    else if (mp3 == 0)
    {
        LOGINFO("loading mp3 file");

        if (mp3dec_ex_open(&stream->mp3, path, MP3D_SEEK_TO_SAMPLE))
        {
            LOGERROR(stringf("could not load audio file '%s': Unknown error"));
            return;
        }

        stream->samples = stream->mp3.samples;
        stream->length = (float)stream->mp3.samples / (float)stream->mp3.info.hz;

        LOGINFO(stringf("sample rate: '%d' | channels: '%d' | frame_bytes: '%d' | length: '%f' | samples: '%d'", stream->mp3.info.hz, stream->mp3.info.channels, stream->mp3.info.frame_bytes, stream->length, stream->samples));

        stream->format = AUDIO_FORMAT_MP3;
    }
    else if (wav == 0)
    {
        LOGINFO("loading wav file");

        stream->wav = wave_open(path, WAVE_OPEN_READ);
        if (wave_get_format(stream->wav) != WAVE_FORMAT_PCM)
        {
            LOGERROR("could not load wav file: expected int16 format");
            wave_close(stream->wav);
            return;
        }

        stream->format = AUDIO_FORMAT_WAV;

        LOGINFO(stringf("sample rate: '%ld' | channels: '%d' | length: '%ld'", wave_get_sample_rate(stream->wav), wave_get_num_channels(stream->wav), wave_get_length(stream->wav)));
    }
    else
        LOGERROR(stringf("could not load audio file '%s': Invalid format"));

    sceKernelDcacheWritebackInvalidateAll();
}

void audio_stream_seek_start(audio_stream_t* astream)
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

void audio_stream_dispose(audio_stream_t* astream)
{
    if (!astream)
        return;

    if (global_audio_stream == astream)
        global_audio_stream = NULL;

    if (astream->format == AUDIO_FORMAT_VORBIS)
        stb_vorbis_close(astream->stream);
    else if (astream->format == AUDIO_FORMAT_MP3)
        mp3dec_ex_close(&astream->mp3);
    else
        wave_close(astream->wav);
}

void audio_callback(void* buffer, unsigned int length, void* userdata)
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

void audio_init(void)
{
    pspAudioInit();
    pspAudioSetChannelCallback(0, audio_callback, NULL);
    audio_set_volume(1.0f);
}

void audio_set_volume(float volume)
{
    if (volume < 0)
        volume = 0;
    
    if (volume > 1)
        volume = 1;

    int vol = (int)(volume * (float)PSP_VOLUME_MAX);
    pspAudioSetVolume(0, vol, vol);
    audioVolume = volume;
}

float audio_get_volume()
{
    return audioVolume;
}

void audio_set_stream(audio_stream_t* astream)
{
    global_audio_stream = astream;
}

void audio_set_end_callback(audio_end_callback_t callback)
{
    audioEndCallback = callback;
}

void audio_dispose()
{
    pspAudioEnd();
}