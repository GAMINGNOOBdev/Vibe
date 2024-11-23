#include <app.h>
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
#include <mainMenu.h>
#include <songSelect.h>
#include <fontRenderer.h>

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

fontRenderer_t* mFontRenderer;
texture_t* mTexture0;
texture_t* mTexture1;
tilemap_t* mFont;
sprite_t* mSprite;
textureAtlas_t mAtlas = {
    .width = 16,
    .height = 16
};

void drawText(tilemap_t* tilemap, const char* str, uint32_t color)
{
    int len = strlen(str);
    memset(tilemap->tiles, 0, sizeof(tile_t)*tilemap->width*tilemap->height);

    for (int i = 0; i < len; i++)
    {
        char c = str[i];

        tile_t tile = {
            .x = i % tilemap->width,
            .y = i / tilemap->height,
            .idx = c,
            .color = color,
        };

        tilemap->tiles[i] = tile;
    }
}

void switchToMainMenu()
{
    setAppUpdateCallback(mainMenuUpdate);
    setAppRenderCallback(mainMenuRender);

    setEndAudioCallback(musicEnded);
    musicEnded();

    drawText(mFont, " Press start", 0xFFFF0000);
    buildTilemap(mFont);

    LOGINFO("loaded main menu");
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

    mFontRenderer = createFontRenderer("Assets", "dos.fnt", 16.f, 0);
    fontRendererBuildText(mFontRenderer, "Hi", 50, 50, 0xFFFFFFFF);

    mFont = createTilemap(&mAtlas, mTexture1, 16, 16);
    mFont->x = 144;
    mFont->y = 16;
    drawText(mFont, " Press start", 0xFFFF0000);
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
    disposeFontRenderer(mFontRenderer);
}

void mainMenuInputHandle(float delta)
{
    if (buttonPressed(PSP_CTRL_UP))
        setAudioVolume(getAudioVolume()+delta);

    if (buttonPressed(PSP_CTRL_DOWN))
        setAudioVolume(getAudioVolume()-delta);

    if (buttonReleased(PSP_CTRL_RIGHT) || buttonReleased(PSP_CTRL_LEFT))
        musicEnded();

    if (buttonPressed(PSP_CTRL_START))
        switchToSongSelect();
}

void mainMenuUpdate(float delta)
{
    mainMenuInputHandle(delta);
}

int frames = 0;

void mainMenuRender()
{
    glDisable(GL_DEPTH_TEST);

    glBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    glEnable(GL_BLEND);

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

    if (frames < 30)
        drawTilemap(mFont);
    else
        fontRendererDraw(mFontRenderer);

    frames++;
    frames %= 60;
}