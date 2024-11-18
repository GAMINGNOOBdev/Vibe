#include <mesh.h>
#include <gu2gl.h>
#include <audio.h>
#include <input.h>
#include <sprite.h>
#include <string.h>
#include <texture.h>
#include <tilemap.h>
#include <logging.h>
#include <pspctrl.h>
#include <fileutil.h>

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
texture_t* mTexture1;
tilemap_t* mFont;
sprite_t* mSprite;
textureAtlas_t mAtlas = {
    .width = 16,
    .height = 16
};

void drawText(tilemap_t* tilemap, const char* str)
{
    int len = strlen(str);
    LOGDEBUG(stringf("string len: '%d'", len));

    for (int i = 0; i < len; i++)
    {
        char c = str[i];

        LOGDEBUG(stringf("current char: '%c' (%d)", c, c));

        tile_t tile = {
            .x = i % tilemap->width,
            .y = i / tilemap->height,
            .idx = c,
        };

        tilemap->tiles[i] = tile;
    }
}

void mainMenuInit()
{
    stringvec_t* assets_contents = fileUtilGetDirectoryContents("Assets/", FilterMaskAllFilesAndFolders);
    for (struct stringvec_entry* entry = assets_contents->start; entry != NULL; entry = entry->next)
    {
        LOGINFO(stringf("PATH: '%s'", entry->contents));
    }
    disposeStringVec(assets_contents);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-16.0f / 9.0f, 16.0f / 9.0f, -1.0f, 1.0f, -10.0f, 10.0f);

    glMatrixMode(GL_VIEW);
    glLoadIdentity();

    glMatrixMode(GL_MODEL);
    glLoadIdentity();

    mTexture0 = loadTexture("Assets/LOGO.png", GL_TRUE, GL_TRUE);
    mTexture1 = loadTexture("Assets/default.png", GL_FALSE, GL_TRUE);

    mFont = createTilemap(&mAtlas, mTexture1, 16, 16);
    mFont->x = 144;
    mFont->y = 16;
    drawText(mFont, " Hello World!");
    buildTilemap(mFont);

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
    disposeTexture(mTexture1);
    disposeTilemap(mFont);
    disposeSprite(mSprite);
}

void mainMenuInputHandle(float delta)
{
    if (buttonPressed(PSP_CTRL_UP))
        setAudioVolume(getAudioVolume()+delta);

    if (buttonPressed(PSP_CTRL_DOWN))
        setAudioVolume(getAudioVolume()-delta);
    
    if (buttonReleased(PSP_CTRL_RIGHT) || buttonReleased(PSP_CTRL_LEFT))
        musicEnded();
}

void mainMenuUpdate(float delta)
{
    mainMenuInputHandle(delta);
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

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 480, 0, 272, -10.0f, 10.0f);

    glMatrixMode(GL_VIEW);
    glLoadIdentity();

    drawTilemap(mFont);
}