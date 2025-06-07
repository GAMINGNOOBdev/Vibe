#ifndef __APP_H_
#define __APP_H_ 1

typedef void(*app_update_callback_t)(float delta);
typedef void(*app_render_callback_t)();

void app_set_update_callback(app_update_callback_t update);
app_update_callback_t app_get_update_callback();

void app_set_render_callback(app_render_callback_t render);
app_render_callback_t app_get_render_callback();

#endif
