#ifndef __CALLBACK_H_
#define __CALLBACK_H_ 1

#ifdef __PSP__
#include <pspkernel.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*cleanup_callback_t)(void);

extern cleanup_callback_t gameCleanupCallback;

void stop_running();
int is_running();
int exit_callback(int arg1, int arg2, void* common);
int setup_callbacks();
void exit_game();

#ifdef __cplusplus
}
#endif

#endif