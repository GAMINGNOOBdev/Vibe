#include <pspkernel.h>
#include <malloc.h>
#include <math.h>

PSP_MODULE_INFO("VIBE", 0, 0, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <callback.h>
#include <logging.h>
#include <audio.h>
#include <gfx.h>

audio_stream* music[3];
int musicIndex = 0;

void musicEnded()
{
    audioStreamSeekStart(music[musicIndex]);
    setAudioStream(music[musicIndex]);
    LOGINFO(stringf("switched to track no.%d", musicIndex));
    musicIndex++;
    musicIndex %= 3;
}

int main()
{
    setupCallback();

    initGraphics();

    remove("game.log");
    FILE* logFile = fopen("game.log", "wb+");
    logSetStream(logFile);
    logEnableDebugMsgs(1);

    initAudio();

    music[0] = loadAudioStream("Assets/tn-shi - Mood Swing.ogg");
    music[1] = loadAudioStream("Assets/tn-shi - lol who cares.ogg");
    music[2] = loadAudioStream("Assets/tn-shi - Contradiction.ogg");

    setEndAudioCallback(musicEnded);
    musicEnded();

    uint64_t frame = 0;

    while (isRunning())
    {
        startFrame();

        sceGuClearColor(0xFF221111);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

        endFrame();
    }

    for (int i = 0; i < 3; i++)
        closeAudioStream(music[i]);

    disposeGraphics();
    disposeAudio();

    fclose(logFile);

    exitGame();

    return 0;
}