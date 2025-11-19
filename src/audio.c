#include <audio.h>
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <minimp3.h>
#include <minimp3_ex.h>

#ifdef __PSP__
#include <pspaudiolib.h>
#include <pspkernel.h>
#include <pspaudio.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <logging.h>
#include <strutil.h>
#include <memory.h>

////////////////////////////////
///                          ///
/// Global defines/functions ///
///                          ///
////////////////////////////////

#ifndef __PSP__
ALenum alError = AL_NO_ERROR;
static const char* alGetErrorString(ALenum error)
{
    if (error == AL_INVALID_NAME)
        return "AL_INVALID_NAME";
    else if (error == AL_INVALID_ENUM)
        return "AL_INVALID_ENUM";
    else if (error == AL_INVALID_VALUE)
        return "AL_INVALID_VALUE";
    else if (error == AL_INVALID_OPERATION)
        return "AL_INVALID_OPERATION";
    else if (error == AL_OUT_OF_MEMORY)
        return "AL_OUT_OF_MEMORY";
    else if (error == AL_INVALID)
        return "AL_INVALID";
    else if (error == AL_INVALID)
        return "AL_INVALID";
    return "AL_UNKNOWN";
}

#define ALCALL(call, ...) alError = alGetError(); call(__VA_ARGS__); if ((alError = alGetError()) != AL_NO_ERROR) {LOGERROR("%s: %s", #call, alGetErrorString(alError));}
#define ALSOURCEUNQUEUEBUFFERS(source, nb, buffers) alError = alGetError(); alSourceUnqueueBuffers(source, nb, buffers); if ((alError = alGetError()) != AL_NO_ERROR) {LOGERROR("alSourceUnqueueBuffers( 0x%x, %d, 0x%x ): %s", source, nb, buffers, alGetErrorString(alError));}
#define ALBUFFERDATA(buffer, format, data, size, samplerate) alError = alGetError(); alBufferData(buffer, format, data, size, samplerate); if ((alError = alGetError()) != AL_NO_ERROR) {LOGERROR("alBufferData( 0x%x, %d, 0x%x, %d, %d ): %s", buffer, format, data, size, samplerate, alGetErrorString(alError));}
#endif

////////////////////////
///                  ///
/// Global variables ///
///                  ///
////////////////////////

typedef struct
{
    uint8_t initialized;
    audio_stream_t* music;
    audio_stream_t* sfx[AUDIO_SFX_STREAMS_MAX];
    #ifndef __PSP__
    ALCdevice* device;
    ALCcontext* context;
    ALuint processed_buffers[AUDIO_BUFFERS_PER_SOURCE];
    #endif
} audio_data_t;

#ifndef _WIN32
audio_data_t audio_data = {0, NULL, {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}};
#else
audio_data_t audio_data = {0};
#endif
float audio_volume = 1.0f;

////////////////////////
///                  ///
/// Global functions ///
///                  ///
////////////////////////

audio_stream_t audio_stream_load(const char* path)
{
    audio_stream_t stream;
    memset(&stream, 0, sizeof(audio_stream_t));
    stream.volume = 1;
    stream.format = AUDIO_FORMAT_INVALID;

    if (!path)
        return stream;

    int lastDot = strlpos(path, '.');
    int vorbis = strcmp(&path[lastDot+1], "ogg");
    int mp3 = strcmp(&path[lastDot+1], "mp3");
    int wav = strcmp(&path[lastDot+1], "wav");

    if (vorbis == 0)
    {
        LOGINFO("loading ogg vorbis file '%s'", path);
        stream.stream = stb_vorbis_open_filename(path, NULL, NULL);
        if (stream.stream == NULL)
        {
            LOGERROR("could not load audio file '%s': Unknown error", path);
            return stream;
        }
        stream.info = stb_vorbis_get_info(stream.stream);
        stream.length = stb_vorbis_stream_length_in_seconds(stream.stream);
        stream.samples = stb_vorbis_stream_length_in_samples(stream.stream);
        stream.freq = stream.info.sample_rate;

        LOGINFO("sample rate: '%d' | channels: '%d' | max_frame_size: '%d' | length: '%f' | samples: '%d'", stream.freq, stream.info.channels, stream.info.max_frame_size, stream.length, stream.samples);

        stream.format = AUDIO_FORMAT_VORBIS;
    }
    else if (mp3 == 0)
    {
        LOGINFO("loading mp3 file '%s'", path);
        if (mp3dec_ex_open(&stream.mp3, path, MP3D_SEEK_TO_SAMPLE))
        {
            LOGERROR("could not load audio file '%s': Unknown error", path);
            return stream;
        }

        stream.samples = stream.mp3.samples;
        stream.freq = stream.mp3.info.hz;
        stream.length = (float)stream.samples / (float)stream.freq / 2.0f;

        LOGINFO("sample rate: '%d' | channels: '%d' | frame_bytes: '%d' | length: '%f' | samples: '%d'", stream.freq, stream.mp3.info.channels, stream.mp3.info.frame_bytes, stream.length, stream.samples);

        stream.format = AUDIO_FORMAT_MP3;
    }
    else if (wav == 0)
    {
        LOGINFO("loading wav file '%s'", path);
        stream.wav = wave_open(path, WAVE_OPEN_READ);
        if (wave_get_format(stream.wav) != WAVE_FORMAT_PCM)
        {
            LOGERROR("could not load wav file '%s': expected int16 format", path);
            wave_close(stream.wav);
            return stream;
        }
        stream.samples = wave_get_length(stream.wav);
        stream.freq = wave_get_sample_rate(stream.wav);
        stream.length = (float)stream.samples / (float)stream.freq;

        stream.format = AUDIO_FORMAT_WAV;

        LOGINFO("sample rate: '%ld' | channels: '%d' | length: '%f'", stream.freq, wave_get_num_channels(stream.wav), stream.length);
    }
    else
    {
        LOGERROR("could not load audio file: Invalid format");
        LOGERROR(path);
    }

    stream.length_ms = stream.length * 1000.0f;

    #ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll();
    #endif

    #ifndef __PSP__
    ALCALL(alGenSources, 1, &stream.source);
    ALCALL(alGenBuffers, AUDIO_BUFFERS_PER_SOURCE, stream.buffers);
    #endif

    stream.playing = 1;
    return stream;
}

uint8_t audio_stream_is_valid(audio_stream_t* astream)
{
    if (!astream)
        return 0;

    return astream->format != AUDIO_FORMAT_INVALID;
}

void audio_stream_update(audio_stream_t* astream)
{
    if (astream == NULL || !audio_stream_is_valid(astream) || !astream->playing)
        return;

    if (astream->processed_frames >= astream->samples)
    {
        if (astream->end_callback != NULL)
            astream->end_callback();

        astream->playing = 0;
        return;
    }

    #ifndef __PSP__
    ALuint source = astream->source;
    ALint processed_buffer_count = 0;
    ALCALL(alGetSourcei, source, AL_BUFFERS_PROCESSED, &processed_buffer_count);
    if (processed_buffer_count <= 0)
        return;

    memset(audio_data.processed_buffers, 0, sizeof(ALuint)*AUDIO_BUFFERS_PER_SOURCE);
    ALSOURCEUNQUEUEBUFFERS(source, processed_buffer_count, audio_data.processed_buffers);

    for (int i = 0; i < processed_buffer_count; i++)
    {
        if (astream == NULL || !audio_stream_is_valid(astream) || !astream->playing)
            return;
        ALuint buffer = audio_data.processed_buffers[i];
        audio_stream_update_buffer(astream, astream->data, AUDIO_FRAME_SIZE);
        ALBUFFERDATA(buffer, AL_FORMAT_STEREO16, astream->data, AUDIO_FRAME_SIZE, astream->freq);
    }

    alSourceQueueBuffers(source, processed_buffer_count, audio_data.processed_buffers);
    #endif

    astream->seconds = (float)astream->processed_frames / (float)astream->samples * 1000.f * astream->length;
}

void audio_stream_update_buffer(audio_stream_t* astream, void* buffer, int bytes)
{
    if (buffer == NULL || bytes == 0)
        return;
    memset(buffer, 0, bytes);

    if (astream == NULL)
        return;
    if (!audio_stream_is_valid(astream))
        return;

    if (astream->processed_frames >= astream->samples)
    {
        if (astream->end_callback != NULL)
            astream->end_callback();

        astream->playing = 0;
        return;
    }

    int frames = 0;
    int expectedFrames = bytes / sizeof(short);
    if (astream->format == AUDIO_FORMAT_VORBIS)
    {
        expectedFrames /= astream->info.channels;
        frames = stb_vorbis_get_samples_short_interleaved(astream->stream, astream->info.channels, (short*)buffer, expectedFrames*sizeof(short));
    }
    else if (astream->format == AUDIO_FORMAT_WAV)
    {
        expectedFrames /= wave_get_sample_size(astream->wav);
        frames = wave_read(astream->wav, buffer, expectedFrames);
    }
    else if (astream->format == AUDIO_FORMAT_MP3)
        frames = mp3dec_ex_read(&astream->mp3, (mp3d_sample_t*)buffer, expectedFrames);

    for (int i = 0; i < expectedFrames; i++)
        ((short*)buffer)[i] *= astream->volume;

    astream->processed_frames += frames;

    if (frames < expectedFrames)
    {
        if(astream->end_callback != NULL)
            astream->end_callback();

        astream->playing = 0;
        return;
    }
}

void audio_stream_pause(audio_stream_t* astream)
{
    if (!astream || !audio_stream_is_valid(astream))
        return;

    astream->playing = 0;
    #ifndef __PSP__
    ALCALL(alSourcePause,astream->source);
    ALuint source = astream->source;
    ALint processed_buffer_count = 0;
    ALint queued_buffer_count = 0;
    ALCALL(alGetSourcei, source, AL_BUFFERS_PROCESSED, &processed_buffer_count);
    ALCALL(alGetSourcei, source, AL_BUFFERS_QUEUED, &queued_buffer_count);
    if (processed_buffer_count > 0)
    {
        memset(audio_data.processed_buffers, 0, sizeof(ALuint)*AUDIO_BUFFERS_PER_SOURCE);
        ALSOURCEUNQUEUEBUFFERS(source, processed_buffer_count, audio_data.processed_buffers);
    }
    LOGDEBUG("paused audio with %d processed and %d queued buffers", processed_buffer_count, queued_buffer_count);
    #endif
}

void audio_stream_resume(audio_stream_t* astream)
{
    if (!astream || !audio_stream_is_valid(astream))
        return;

    astream->playing = 1;
    #ifndef __PSP__
    ALuint source = astream->source;
    ALint processed_buffer_count = 0;
    ALint queued_buffer_count = 0;
    ALCALL(alGetSourcei, source, AL_BUFFERS_PROCESSED, &processed_buffer_count);
    ALCALL(alGetSourcei, source, AL_BUFFERS_QUEUED, &queued_buffer_count);
    LOGDEBUG("resuming audio with %d processed and %d queued buffers", processed_buffer_count, queued_buffer_count);
    ALCALL(alSourcePlay,astream->source);
    #endif
}

void audio_stream_seek_start(audio_stream_t* astream)
{
    if (!astream)
        return;

    if (astream->format == AUDIO_FORMAT_VORBIS)
    {
        stb_vorbis_seek_start(astream->stream);
        LOGDEBUG("vorbis stream seeked to start");
    }
    else if (astream->format == AUDIO_FORMAT_MP3)
    {
        mp3dec_ex_seek(&astream->mp3, 0);
        LOGDEBUG("mp3 stream seeked to start");
    }
    else if (astream->format == AUDIO_FORMAT_WAV)
    {
        wave_seek(astream->wav, 0, 0);
        LOGDEBUG("wav stream seeked to start");
    }
    
    astream->processed_frames = 0;
    astream->seconds = 0;
}

void audio_stream_dispose(audio_stream_t* astream)
{
    if (!astream)
        return;

    astream->playing = 0;
    #ifndef __PSP__
    ALCALL(alSourceStop, astream->source);
    ALint processed_buffer_count = 0;
    ALCALL(alGetSourcei, astream->source, AL_BUFFERS_PROCESSED, &processed_buffer_count);
    if (processed_buffer_count > 0)
    {
        memset(audio_data.processed_buffers, 0, sizeof(ALuint)*AUDIO_BUFFERS_PER_SOURCE);
        ALSOURCEUNQUEUEBUFFERS(astream->source, processed_buffer_count, audio_data.processed_buffers);
    }
    ALCALL(alDeleteBuffers, AUDIO_BUFFERS_PER_SOURCE, astream->buffers);
    ALCALL(alDeleteSources, 1, &astream->source);
    #endif

    if (audio_data.music == astream)
        audio_data.music = NULL;
    for (int i = 0; i < AUDIO_SFX_STREAMS_MAX; i++)
        if (audio_data.sfx[i] == astream)
            audio_data.sfx[i] = NULL;

    if (astream->format == AUDIO_FORMAT_VORBIS)
        stb_vorbis_close(astream->stream);
    else if (astream->format == AUDIO_FORMAT_MP3)
        mp3dec_ex_close(&astream->mp3);
    else if (astream->format == AUDIO_FORMAT_WAV)
        wave_close(astream->wav);

    memset(astream, 0, sizeof(audio_stream_t));
}

static void psp_audio_fill_buffer_callback(void* buffer, unsigned int samples, void* userdata)
{
    uint32_t bytes = sizeof(short) * samples * 2;
    memset(buffer, 0, bytes);
    audio_stream_t* astream = *(audio_stream_t**)userdata;
    if (astream == NULL)
        return;

    if (!astream->playing)
        return;

    audio_stream_update_buffer(astream, buffer, bytes);
    audio_stream_update(astream);
}

#ifndef __PSP__
ALCdevice* audio_choose_device(void)
{
    ALCdevice* device = NULL;

    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_FALSE)
    {
        LOGWARNING("Cannot find multiple devices, choosing default device");
        return alcOpenDevice(NULL);
    }

    const char* defaultDevice = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    const char* devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
    LOGDEBUG("Default device name: '%s'", defaultDevice);

    char* deviceNamePtr = (char*)devices;
    while (*deviceNamePtr != 0)
    {
        LOGDEBUG("Device name: '%s'", deviceNamePtr);
        deviceNamePtr += strlen(deviceNamePtr)+1;
    }

    device = alcOpenDevice(defaultDevice);
    LOGINFO("Found suitable device");
    return device;
}
#endif // #ifndef __PSP__

void audio_init(void)
{
    if (audio_data.initialized)
        return;

    #ifdef __PSP__
    pspAudioInit();
    pspAudioSetChannelCallback(0, psp_audio_fill_buffer_callback, (void*)&audio_data.music);
    for (size_t i = 0; i < AUDIO_SFX_STREAMS_MAX; i++)
        pspAudioSetChannelCallback(1+i, psp_audio_fill_buffer_callback, (void*)&audio_data.sfx[i]);
    #else
    audio_data.device = audio_choose_device();
    if (!audio_data.device)
    {
        LOGERROR("Unable to find a suitable audio device");
        return;
    }

    audio_data.context = alcCreateContext(audio_data.device, NULL);
    alcMakeContextCurrent(audio_data.context);

    ALCALL(alListenerf, AL_GAIN, 1.0f);
    ALCALL(alListener3f, AL_POSITION, 0, 0, 0);
    ALCALL(alListener3f, AL_VELOCITY, 0, 0, 0);
    ALfloat listenerOrientation[] = {
        0, 0, 1, 1, 1, 0
    };
    ALCALL(alListenerfv, AL_ORIENTATION, listenerOrientation);

    #endif
    audio_set_volume(1.0f);

    audio_data.initialized = 1;
}

void audio_update(void)
{
    for (int i = 0; i < AUDIO_STREAMS_MAX; i++)
    {
        audio_stream_t* stream = audio_data.music;
        if (i != 0)
            stream = audio_data.sfx[i-1];

        audio_stream_update(stream);
    }
}

void audio_set_volume(float volume)
{
    if (volume < 0)
        volume = 0;
    
    if (volume > 1)
        volume = 1;

    #ifdef __PSP__
    int vol = (int)(volume * (float)PSP_VOLUME_MAX);
    pspAudioSetVolume(0, vol, vol);
    #else
    ALCALL(alListenerf, AL_GAIN, volume);
    #endif
    audio_volume = volume;
}

float audio_get_volume(void)
{
    return audio_volume;
}

void audio_set_music_stream(audio_stream_t* astream)
{
    #ifndef __PSP__
    if (audio_data.music)
    {
        ALuint source = audio_data.music->source;
        ALint processed_buffer_count = 0;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed_buffer_count);
        if (processed_buffer_count > 0)
        {
            ALSOURCEUNQUEUEBUFFERS(source, AUDIO_BUFFERS_PER_SOURCE, audio_data.processed_buffers);
        }
        ALCALL(alSourceStop, source);
    }
    #endif

    audio_data.music = astream;
    if (astream == NULL)
        return;

    #ifndef __PSP__
    ALuint source = astream->source;
    for (int i = 0; i < AUDIO_BUFFERS_PER_SOURCE; i++)
    {
        ALuint buffer = astream->buffers[i];
        audio_stream_update_buffer(astream, astream->data, AUDIO_FRAME_SIZE);
        ALCALL(alBufferData, buffer, AL_FORMAT_STEREO16, astream->data, AUDIO_FRAME_SIZE, astream->freq);
    }

    ALCALL(alSourceQueueBuffers, source, AUDIO_BUFFERS_PER_SOURCE, astream->buffers);

    alSourcePlay(source);
    #endif
    astream->playing = 1;
}

void audio_play_sfx_stream(audio_stream_t* astream)
{
    for (uint8_t i = 0; i < AUDIO_SFX_STREAMS_MAX; i++)
    {
        if (audio_data.sfx[i] != NULL)
            continue;

        audio_stream_seek_start(astream);
        audio_data.sfx[i] = astream;
        #ifndef __PSP__
        ALuint source = astream->source;
        ALint processed_buffer_count = 0;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed_buffer_count);
        if (processed_buffer_count > 0)
        {
            ALSOURCEUNQUEUEBUFFERS(source, AUDIO_BUFFERS_PER_SOURCE, audio_data.processed_buffers);
        }
        ALCALL(alSourceStop, source);

        if (astream == NULL)
            return;

        for (int j = 0; j < AUDIO_BUFFERS_PER_SOURCE; j++)
        {
            ALuint buffer = astream->buffers[j];
            audio_stream_update_buffer(astream, astream->data, AUDIO_FRAME_SIZE);
            ALCALL(alBufferData, buffer, AL_FORMAT_STEREO16, astream->data, AUDIO_FRAME_SIZE, astream->freq);
        }

        ALCALL(alSourceQueueBuffers, source, AUDIO_BUFFERS_PER_SOURCE, astream->buffers);
        alSourcePlay(source);
        #endif

        return;
    }
    LOGERROR("unable to play sfx 0x%16.16x: not enough audio channels", astream);
}

void audio_dispose(void)
{
    if (!audio_data.initialized)
        return;

    LOGINFO("closing audio engine");
    #ifdef __PSP__
    pspAudioEnd();
    #else
    alcMakeContextCurrent(NULL);
    alcDestroyContext(audio_data.context);
    alcCloseDevice(audio_data.device);
    #endif
}
