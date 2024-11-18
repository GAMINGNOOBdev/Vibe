#include <callback.h>

static int ExitRequest = 0;

int isRunning()
{
    return !ExitRequest;
}

int exitCallback(int arg1, int arg2, void* common)
{
    ExitRequest = 1;
    return 0;
}

int callbackThread(SceSize args, void* argsPtr)
{
    int callBackID;
    callBackID = sceKernelCreateCallback("Exit Callback", exitCallback, NULL);
    sceKernelRegisterExitCallback(callBackID);
    sceKernelSleepThreadCB();
    return 0;
}

int setupCallback()
{
    int threadID = 0;
    threadID = sceKernelCreateThread("Callback Update Thread", callbackThread, 0x11, 0xFA0, THREAD_ATTR_USER, 0);
    if (threadID >= 0)
        sceKernelStartThread(threadID, 0, 0);

    return threadID;
}

void exitGame()
{
    sceKernelExitGame();
}