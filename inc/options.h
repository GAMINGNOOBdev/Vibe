#ifndef __OPTIONS_H_
#define __OPTIONS_H_ 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCROLL_SPEED_MAX 40
#define SCROLL_SPEED_MIN 1

typedef struct
{
    uint8_t show_fps : 1;
    uint8_t show_debug_info : 1;
    uint8_t enable_debug_log : 1;

    uint8_t reserved : 5;
} options_flags_t;

typedef struct
{
    // mania 4 key lane x
    int m4l1, m4l2, m4l3, m4l4;
} options_keybinds_t;

typedef struct
{
    float master_volume;
    float music_volume;
    float hitsound_volume;
    uint8_t scroll_speed;
    options_flags_t flags;
    options_keybinds_t keybinds;
} options_t;

extern options_t options;

void options_load(void);
void options_apply(void);
void options_save(void);

#ifdef __cplusplus
}
#endif

#endif