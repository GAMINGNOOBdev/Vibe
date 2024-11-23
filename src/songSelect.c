#include <app.h>
#include <audio.h>
#include <gu2gl.h>
#include <input.h>
#include <string.h>
#include <malloc.h>
#include <logging.h>
#include <pspctrl.h>
#include <tilemap.h>
#include <fileutil.h>
#include <songlist.h>
#include <mainMenu.h>
#include <songSelect.h>

extern tilemap_t* mFont;
extern void drawText(tilemap_t* tilemap, const char* str, uint32_t color);

void switchToSongSelect()
{
    setAppUpdateCallback(songSelectUpdate);
    setAppRenderCallback(songSelectRender);

    setEndAudioCallback(NULL);
    setAudioStream(NULL);

    drawText(mFont, " ", 0xFFFFFFFF);
    buildTilemap(mFont);

    LOGINFO("loaded song select menu");
}

songlist_t* songList = NULL;
struct songlist_entry_t* songListCurrent = NULL;

void selectedSongUpdated()
{
    if (songListCurrent == NULL)
        return;

    char* string = stringf("'%s' by '%s' (%lld)", songListCurrent->songname, songListCurrent->artist, songListCurrent->id);
    size_t stringLen = strlen(string);
    char* str = malloc(stringLen+1);
    if (!str)
        return;
    memset(str, 0, stringLen+1);
    memcpy(str, string, stringLen);
    drawText(mFont, str, 0xFFFFFFFF);
    free(str);
    buildTilemap(mFont);
}

void songSelectInit()
{
    songList = createSongList("Songs");

    if (songList != NULL)
        songListCurrent = songList->start;

    selectedSongUpdated();
}

void songSelectDispose()
{
    disposeSongList(songList);
}

void songSelectInputHandle(float delta)
{
    // if (buttonPressed(PSP_CTRL_CROSS));

    if (buttonReleased(PSP_CTRL_DOWN))
    {
        if (songListCurrent->next == NULL)
            songListCurrent = songList->start;
        else
            songListCurrent = songListCurrent->next;
        
        selectedSongUpdated();
    }

    if (buttonReleased(PSP_CTRL_UP))
    {
        if (songListCurrent->prev == NULL)
            songListCurrent = songList->end;
        else
            songListCurrent = songListCurrent->prev;

        selectedSongUpdated();
    }
}

void songSelectUpdate(float delta)
{
    songSelectInputHandle(delta);
}

void songSelectRender()
{
    glDisable(GL_DEPTH_TEST);

    glBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    glEnable(GL_BLEND);

    glClearColor(0xFF221111);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 480, 0, 272, -10.0f, 10.0f);

    glMatrixMode(GL_VIEW);
    glLoadIdentity();

    drawTilemap(mFont);
}