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
#include <malloc.h>
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
    #ifndef __PSP__
    ALCdevice* device;
    ALCcontext* context;
    ALuint sources[AUDIO_STREAMS_MAX];
    ALuint buffers[AUDIO_STREAMS_MAX][AUDIO_BUFFERS_PER_SOURCE];
    short data_buffers[AUDIO_STREAMS_MAX][AUDIO_FRAME_SIZE];
    ALuint processed_buffers[AUDIO_BUFFERS_PER_SOURCE];
    #endif
} audio_data_t;

audio_stream_t* sfx_audio_streams[AUDIO_SFX_STREAMS_MAX];
audio_stream_t* music_audio_stream = NULL;
audio_data_t audio_data = {0};
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

    return stream;
}

uint8_t audio_stream_is_valid(audio_stream_t* astream)
{
    if (!astream)
        return 0;

    return astream->format != AUDIO_FORMAT_INVALID;
}

void audio_stream_update(audio_stream_t* astream, int index)
{
    if (astream == NULL || !audio_stream_is_valid(astream))
        return;

    if (astream->processed_frames >= astream->samples)
    {
        if (astream->end_callback != NULL)
            astream->end_callback();

        return;
    }

    #ifndef __PSP__
    ALint source_state = 0;
    ALuint source = audio_data.sources[index];
    ALCALL(alGetSourcei, source, AL_SOURCE_STATE, &source_state);
    astream->playing = source_state == AL_PLAYING;

    ALint processed_buffer_count = 0;
    ALCALL(alGetSourcei, source, AL_BUFFERS_PROCESSED, &processed_buffer_count);
    if (processed_buffer_count <= 0)
        return;

    memset(audio_data.processed_buffers, 0, sizeof(ALuint)*AUDIO_BUFFERS_PER_SOURCE);
    ALSOURCEUNQUEUEBUFFERS(source, processed_buffer_count, audio_data.processed_buffers);

    for (int i = 0; i < processed_buffer_count; i++)
    {
        ALuint buffer = audio_data.processed_buffers[i];
        audio_stream_update_buffer(astream, audio_data.data_buffers[index], AUDIO_FRAME_SIZE);
        ALBUFFERDATA(buffer, AL_FORMAT_STEREO16, audio_data.data_buffers[index], AUDIO_FRAME_SIZE, astream->freq);
    }

    alSourceQueueBuffers(source, processed_buffer_count, audio_data.processed_buffers);
    #endif

    astream->seconds = (float)astream->processed_frames / (float)astream->samples * 1000.f * astream->length;
}

void audio_stream_update_buffer(audio_stream_t* astream, void* buffer, int bytes)
{
    memset(buffer, 0, bytes);
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

    if (music_audio_stream == astream)
        music_audio_stream = NULL;

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
    int index = (int)userdata;
    audio_stream_t* astream = music_audio_stream;
    if (index != 0)
        astream = sfx_audio_streams[index - 1];
    if (astream == NULL)
        return;

    audio_stream_update_buffer(astream, buffer, bytes);
    audio_stream_update(astream, index);
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
    for (size_t i = 0; i < AUDIO_STREAMS_MAX; i++)
        pspAudioSetChannelCallback(i, psp_audio_fill_buffer_callback, (void*)i);
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

    ALCALL(alGenSources, AUDIO_STREAMS_MAX, audio_data.sources);
    for (int i = 0; i < AUDIO_STREAMS_MAX; i++)
    {
        ALuint* bufferptr = audio_data.buffers[i];
        ALCALL(alGenBuffers, AUDIO_BUFFERS_PER_SOURCE, bufferptr);
    }
    #endif
    audio_set_volume(1.0f);

    audio_data.initialized = 1;
}

void audio_update(void)
{
    for (int i = 0; i < AUDIO_STREAMS_MAX; i++)
    {
        audio_stream_t* stream = music_audio_stream;
        if (i != 0)
            stream = sfx_audio_streams[i-1];

        audio_stream_update(stream, i);
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
    ALCALL(alListenerf, AL_GAIN, 1.0f);
    #endif
    audio_volume = volume;
}

float audio_get_volume(void)
{
    return audio_volume;
}

void audio_set_music_stream(audio_stream_t* astream)
{
    music_audio_stream = astream;
    #ifndef __PSP__
    ALuint source = audio_data.sources[0];
    ALint processed_buffer_count = 0;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed_buffer_count);
    if (processed_buffer_count > 0)
    {
        ALSOURCEUNQUEUEBUFFERS(source, AUDIO_BUFFERS_PER_SOURCE, audio_data.processed_buffers);
    }
    ALCALL(alSourceStop, source);

    if (astream == NULL)
        return;

    for (int i = 0; i < AUDIO_BUFFERS_PER_SOURCE; i++)
    {
        ALuint buffer = audio_data.buffers[0][i];
        audio_stream_update_buffer(astream, audio_data.data_buffers[0], AUDIO_FRAME_SIZE);
        ALCALL(alBufferData, buffer, AL_FORMAT_STEREO16, audio_data.data_buffers[0], AUDIO_FRAME_SIZE, astream->freq);
    }

    ALCALL(alSourceQueueBuffers, source, AUDIO_BUFFERS_PER_SOURCE, audio_data.buffers[0]);

    alSourcePlay(source);
    #endif
}

void audio_play_sfx_stream(audio_stream_t* astream)
{
    for (uint8_t i = 0; i < AUDIO_SFX_STREAMS_MAX; i++)
    {
        if (sfx_audio_streams[i] != NULL)
            continue;

        audio_stream_seek_start(astream);
        sfx_audio_streams[i] = astream;
        #ifndef __PSP__
        int idx = i + 1;
        ALuint source = audio_data.sources[idx];
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
            ALuint buffer = audio_data.buffers[i][j];
            audio_stream_update_buffer(astream, audio_data.data_buffers[i], AUDIO_FRAME_SIZE);
            ALCALL(alBufferData, buffer, AL_FORMAT_STEREO16, audio_data.data_buffers[i], AUDIO_FRAME_SIZE, astream->freq);
        }

        ALCALL(alSourceQueueBuffers, source, AUDIO_BUFFERS_PER_SOURCE, audio_data.buffers[i]);
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

    #ifdef __PSP__
    pspAudioEnd();
    #else
    for (int i = 0; i < AUDIO_STREAMS_MAX; i++)
    {
        ALCALL(alDeleteBuffers, AUDIO_BUFFERS_PER_SOURCE, audio_data.buffers[i]);
    }
    ALCALL(alDeleteSources, AUDIO_STREAMS_MAX, audio_data.sources);

    alcMakeContextCurrent(NULL);
    alcDestroyContext(audio_data.context);
    alcCloseDevice(audio_data.device);
    #endif
}
