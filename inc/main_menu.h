#ifndef __MAIN_MENU_H_
#define __MAIN_MENU_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

void switch_to_main_menu(void);

void main_menu_init(void);
void main_menu_dispose(void);
void main_menu_update(float delta);
void main_menu_render(void);

#ifdef __cplusplus
}
#endif

#endif