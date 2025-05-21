#include <callback.h>
#include <options.h>
#include <stddef.h>

#ifndef __PSP__
#include <SDL2/SDL.h>
#endif

cleanup_callback_t gameCleanupCallback = NULL;
int ExitRequest = 0;

void stop_running()
{
    ExitRequest = 1;
}

int is_running()
{
    return ExitRequest == 0;
}

int exit_callback(int arg1, int arg2, void* common)
{
    if (gameCleanupCallback != NULL)
        gameCleanupCallback();
    options_save();
    stop_running();
    return 0;
}

#ifdef __PSP__
int callback_thread(SceSize args, void* argsPtr)
{
    int callBackID;
    callBackID = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(callBackID);
    sceKernelSleepThreadCB();
    return 0;
}
#endif

int setup_callbacks()
{
    #ifdef __PSP__
    int threadID = 0;
    threadID = sceKernelCreateThread("Callback Update Thread", callback_thread, 0x11, 0xFA0, THREAD_ATTR_USER, 0);
    if (threadID >= 0)
        sceKernelStartThread(threadID, 0, 0);

    return threadID;
    #else
    return 0;
    #endif
}

void exit_game()
{
    #ifdef __PSP__
    sceKernelExitGame();
    #else
    SDL_Quit();
    #endif
}