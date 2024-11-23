#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <malloc.h>
#include <math.h>

PSP_MODULE_INFO("VIBE", 0, 0, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <songSelect.h>
#include <callback.h>
#include <mainMenu.h>
#include <logging.h>
#include <audio.h>
#include <input.h>
#include <time.h>
#include <gfx.h>
#include <app.h>

appUpdateCallback_t mUpdate = NULL;
appRenderCallback_t mRender = NULL;

void setAppUpdateCallback(appUpdateCallback_t update) { mUpdate = update; }
appUpdateCallback_t getAppUpdateCallback() { return mUpdate; }

void setAppRenderCallback(appRenderCallback_t render) { mRender = render; }
appRenderCallback_t getAppRenderCallback() { return mRender; }

int main()
{
    setupCallback();

    pspDebugScreenInit();
    pspDebugScreenPrintf("loading...");

    initGraphics();
    inputEnable(PSP_CTRL_MODE_ANALOG);
    timeInit();

    remove("game.log");
    FILE* logFile = fopen("game.log", "wb+");
    logSetStream(logFile);
    logEnableDebugMsgs(1);

    initAudio();

    mainMenuInit();
    songSelectInit();

    switchToMainMenu();

    while (isRunning())
    {
        inputRead();

        if (mUpdate)
            mUpdate(timeDelta());

        startFrame();

        if (mRender)
            mRender();

        endFrame();

        timeTick();
    }

    songSelectDispose();
    mainMenuDispose();

    disposeGraphics();
    disposeAudio();

    fclose(logFile);

    exitGame();

    return 0;
}