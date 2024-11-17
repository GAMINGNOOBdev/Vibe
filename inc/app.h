#ifndef __APPLICATION_H_
#define __APPLICATION_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*appUpdateCallback_t)(float delta);
typedef void(*appRenderCallback_t)();

void setAppUpdateCallback(appUpdateCallback_t update);
appUpdateCallback_t getAppUpdateCallback();

void setAppRenderCallback(appRenderCallback_t render);
appRenderCallback_t getAppRenderCallback();

#ifdef __cplusplus
}
#endif

#endif