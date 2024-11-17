#include <gu2gl.h>
#include <audio.h>
#include <logging.h>

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

void mainMenuInit()
{
    music[0] = loadAudioStream("Assets/tn-shi - Mood Swing.ogg");
    music[1] = loadAudioStream("Assets/tn-shi - lol who cares.ogg");
    music[2] = loadAudioStream("Assets/tn-shi - Contradiction.ogg");

    setEndAudioCallback(musicEnded);
    musicEnded();
}

void mainMenuDispose()
{
    for (int i = 0; i < 3; i++)
        closeAudioStream(music[i]);
}

void mainMenuUpdate(float delta)
{
}

uint64_t frame = 0;

void mainMenuRender()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glClearColor(0xFF221111 + frame % 238);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    frame++;
}