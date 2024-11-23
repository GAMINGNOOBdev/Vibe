#ifndef __MAINMENU_H_
#define __MAINMENU_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

void switchToMainMenu();

void mainMenuInit();
void mainMenuDispose();
void mainMenuUpdate(float delta);
void mainMenuRender();

#ifdef __cplusplus
}
#endif

#endif