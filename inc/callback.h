#ifndef __CALLBACK_H_
#define __CALLBACK_H_ 1

#include <pspkernel.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*cleanup_callback_t)(void);

extern cleanup_callback_t gameCleanupCallback;

int isRunning();
int exitCallback(int arg1, int arg2, void* common);
int callbackThread(SceSize args, void* argsPtr);
int setupCallback();
void exitGame();

#ifdef __cplusplus
}
#endif

#endif