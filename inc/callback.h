#ifndef __CALLBACK_H_
#define __CALLBACK_H_ 1

#include <pspkernel.h>

#ifdef __cplusplus
extern "C"
{
#endif

int isRunning();
int exitCallback(int arg1, int arg2, void* common);
int callbackThread(SceSize args, void* argsPtr);
int setupCallback();
void exitGame();

#ifdef __cplusplus
}
#endif

#endif