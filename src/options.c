#include "logging.h"
#include <options.h>
#include <input.h>
#include <stdio.h>
#include <audio.h>

#ifndef __PSP__
#include <pctypes.h>
#endif

options_t options = {
    .master_volume = 1,
    .music_volume = 1,
    .hitsound_volume = 1,
    .scroll_speed = SCROLL_SPEED_MAX,
    .flags = {
        .show_fps = 0,
        .show_debug_info = 0,
        .enable_debug_log = 1,
        .reserved = 0,
    },
    .keybinds = {
        .m4l1 = PSP_CTRL_LEFT,
        .m4l2 = PSP_CTRL_LTRIGGER,
        .m4l3 = PSP_CTRL_RTRIGGER,
        .m4l4 = PSP_CTRL_CIRCLE,
    }
};

void options_load(void)
{
    FILE* in = NULL;
    #ifdef __PSP__
    in = fopen("options.bin", "rb");
    #else
    in = fopen("options_pc.bin", "rb");
    #endif
    if (in == NULL)
    {
        options_save();
        return;
    }

    fread(&options, sizeof(options_t), 1, in);

    fclose(in);
}

void options_apply()
{
    LOGDEBUG(stringf("Master volume: %2.2f", options.master_volume));
    audio_set_volume(options.master_volume);
    log_enable_debug_messages(options.flags.enable_debug_log);
}

void options_save(void)
{
    FILE* out = NULL;
    #ifdef __PSP__
    out = fopen("options.bin", "wb+");
    #else
    out = fopen("options_pc.bin", "wb+");
    #endif
    if (out == NULL)
        return;

    fwrite(&options, sizeof(options_t), 1, out);
    char pad = 0;
    fwrite(&pad, 1, 1, out);

    fclose(out);

    LOGDEBUG("saved options");
}
