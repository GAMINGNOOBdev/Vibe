#include <mesh.h>
#include <gu2gl.h>
#include <audio.h>
#include <sprite.h>
#include <texture.h>
#include <logging.h>

audio_stream_t* music[3];
int musicIndex = 0;

void musicEnded()
{
    audioStreamSeekStart(music[musicIndex]);
    setAudioStream(music[musicIndex]);
    LOGINFO(stringf("switched to track no.%d", musicIndex));
    musicIndex++;
    musicIndex %= 3;
}

texture_t* mTexture0;
sprite_t* mSprite;

void mainMenuInit()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-16.0f / 9.0f, 16.0f / 9.0f, -1.0f, 1.0f, -10.0f, 10.0f);

    glMatrixMode(GL_VIEW);
    glLoadIdentity();

    glMatrixMode(GL_MODEL);
    glLoadIdentity();

    mTexture0 = loadTexture("Assets/LOGO.png", GL_TRUE, GL_TRUE);
    mSprite = createSprite(0, 0, 1, 1, mTexture0);

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

    disposeTexture(mTexture0);
    disposeSprite(mSprite);
}

void mainMenuUpdate(float delta)
{
}

void mainMenuRender()
{
    glDisable(GL_DEPTH_TEST);

    glClearColor(0xFF221111);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ///////////////////////
    ///                 ///
    ///   NORMAL PASS   ///
    ///                 ///
    ///////////////////////

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-16.0f / 9.0f, 16.0f / 9.0f, -1.0f, 1.0f, -10.0f, 10.0f);

    drawSprite(mSprite);

    ///////////////////
    ///             ///
    ///   UI PASS   ///
    ///             ///
    ///////////////////

    ///TODO: ui pass
}