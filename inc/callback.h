#ifndef __CALLBACK_H_
#define __CALLBACK_H_ 1

#include <pspkernel.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*cleanup_callback_t)(void);

extern cleanup_callback_t gameCleanupCallback;

int is_running();
int exit_callback(int arg1, int arg2, void* common);
int callback_thread(SceSize args, void* argsPtr);
int setup_callbacks();
void exit_game();

#ifdef __cplusplus
}
#endif

#endif