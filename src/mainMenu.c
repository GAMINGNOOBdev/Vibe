#include <app.h>
#include <mesh.h>
#include <gu2gl.h>
#include <audio.h>
#include <input.h>
#include <sprite.h>
#include <string.h>
#include <options.h>
#include <texture.h>
#include <tilemap.h>
#include <logging.h>
#include <pspctrl.h>
#include <pspaudio.h>
#include <fileutil.h>
#include <mainMenu.h>
#include <songSelect.h>
#include <fontRenderer.h>

audio_stream_t* mainMenuMusic;

void mainMenuMusicEnded()
{
    audioStreamSeekStart(mainMenuMusic);
    setAudioStream(mainMenuMusic);
}

// fontRenderer_t* mFontRenderer;
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

    if (getOptions()->playMenuMusic)
    {
        setEndAudioCallback(mainMenuMusicEnded);
        mainMenuMusicEnded();
    }

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
    glMatrixMode(GL_VIEW);
    glLoadIdentity();
    glMatrixMode(GL_MODEL);
    glLoadIdentity();

    mTexture0 = loadTexture("Assets/mainMenu.png", GL_TRUE, GL_TRUE);
    mTexture1 = loadTexture("Assets/default.png", GL_FALSE, GL_TRUE);

    // mFontRenderer = createFontRenderer("Assets", "dos.fnt", 8.f, 0);
    // fontRendererBuildText(mFontRenderer, "Hi", 50, 50, 0xFFFFFFFF);

    mFont = createTilemap(&mAtlas, mTexture1, 16, 16);
    mFont->x = 144;
    mFont->y = 16;
    drawText(mFont, " Press start", 0xFFFFFFFF);
    buildTilemap(mFont);

    mSprite = createSprite(0, 0.5, 4, 4, mTexture0);

    mainMenuMusic = loadAudioStream("Assets/mainMenu.wav");
}

void mainMenuDispose()
{
    closeAudioStream(mainMenuMusic);

    disposeTexture(mTexture0);
    disposeTexture(mTexture1);
    disposeTilemap(mFont);
    disposeSprite(mSprite);
    // disposeFontRenderer(mFontRenderer);
}

void mainMenuInputHandle(float delta)
{
    if (buttonPressed(PSP_CTRL_UP))
    {
        setAudioVolume(getAudioVolume()+delta);
        getOptions()->audioMasterVolume = getAudioVolume()*PSP_AUDIO_VOLUME_MAX;
    }

    if (buttonPressed(PSP_CTRL_DOWN))
    {
        setAudioVolume(getAudioVolume()-delta);
        getOptions()->audioMasterVolume = getAudioVolume()*PSP_AUDIO_VOLUME_MAX;
    }

    if (buttonPressed(PSP_CTRL_TRIANGLE))
        getOptions()->playMenuMusic = !getOptions()->playMenuMusic;

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

    glClearColor(0xFF111111);
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
    // else
    //     fontRendererDraw(mFontRenderer);

    frames++;
    frames %= 60;
}