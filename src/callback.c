#include <callback.h>

cleanup_callback_t gameCleanupCallback = NULL;
int ExitRequest = 0;

int is_running()
{
    return ExitRequest == 0;
}

int exit_callback(int arg1, int arg2, void* common)
{
    if (gameCleanupCallback != NULL)
        gameCleanupCallback();
    ExitRequest = 1;
    return 0;
}

int callback_thread(SceSize args, void* argsPtr)
{
    int callBackID;
    callBackID = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(callBackID);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks()
{
    int threadID = 0;
    threadID = sceKernelCreateThread("Callback Update Thread", callback_thread, 0x11, 0xFA0, THREAD_ATTR_USER, 0);
    if (threadID >= 0)
        sceKernelStartThread(threadID, 0, 0);

    return threadID;
}

void exit_game()
{
    sceKernelExitGame();
}