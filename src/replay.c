#include <logging.h>
#include <options.h>
#include <replay.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <input.h>
#include <stdio.h>

#ifdef __PSP__
#include <pspkernel.h>
#endif

#ifdef __PSP__
#define REPLAY_BUFFER_SIZE 0x10
#else
#define REPLAY_BUFFER_SIZE 0x100
#endif
void replay_initialize(replay_t* replay)
{
    if (replay == NULL)
        return;

    memset(replay, 0, sizeof(replay_t));
}

void replay_clear(replay_t* replay)
{
    if (replay == NULL)
        return;

    replay->count = 0;
}

static void replay_add_input(replay_t* replay, float timestamp, uint8_t action, int input)
{
    if (replay->count >= replay->buffer_size)
    {
        replay->buffer_size += REPLAY_BUFFER_SIZE;
        replay->frames = realloc(replay->frames, replay->buffer_size * sizeof(replay_frame_t));

        #ifdef __PSP__
        sceKernelDcacheWritebackInvalidateAll();
        #endif
    }

    replay->frames[replay->count] = (replay_frame_t){.timestamp = timestamp, .action = action, .key = input};
    replay->count++;
}

void replay_record_inputs(replay_t* replay, float timestamp)
{
    if (replay == NULL || timestamp < 0)
        return;

    static uint8_t current_key_state[] = {
        0, 0, 0, 0
    };
    static uint8_t old_key_state[] = {
        0, 0, 0, 0
    };

    for (int i = 0; i < 4; i++)
    {
        int key = options.game_keybinds.m4l1;
        if (i == 1)
            key = options.game_keybinds.m4l2;
        else if (i == 2)
            key = options.game_keybinds.m4l3;
        else if (i == 3)
            key = options.game_keybinds.m4l4;

        current_key_state[i] = button_pressed(key);
        if (current_key_state[i] != old_key_state[i])
            replay_add_input(replay, timestamp, current_key_state[i], key);
        old_key_state[i] = current_key_state[i];
    }
}

void replay_playback(replay_t* replay, float timestamp)
{
    if (replay == NULL || timestamp < 0)
        return;

    LOGINFO("WIP");
}

void replay_save(replay_t* replay, score_t* score, uint8_t save_playback_data, const char* filepath)
{
    if (score == NULL || filepath == NULL)
        return;

    if (save_playback_data && replay == NULL)
        return;

    LOGINFO("Saving replay & score to '%s'", filepath);
    FILE* file = fopen(filepath, "w+");
    if (file == NULL)
    {
        LOGERROR("Cannot save replay to file '%s'", filepath);
        return;
    }

    LOGDEBUG("saving replay score...");
    fwrite(score, sizeof(score_t), 1, file);
    fwrite(&save_playback_data, 1, 1, file);
    if (save_playback_data)
    {
        fwrite(&replay->count, sizeof(size_t), 1, file);
        fwrite(replay->frames, sizeof(replay_frame_t), replay->count, file);
        LOGDEBUG("saved replay data with %ld input frames", replay->count);
    }

    fclose(file);
}

void replay_load(replay_t* replay, score_t* score, const char* filepath)
{
    if (score == NULL || filepath == NULL)
        return;

    LOGINFO("Loading replay & score from '%s'", filepath);
    FILE* file = fopen(filepath, "r");
    if (file == NULL)
    {
        LOGERROR("Cannot replay file '%s'", filepath);
        return;
    }

    LOGDEBUG("loading replay score...");
    fread(score, sizeof(score_t), 1, file);
    if (!replay)
    {
        fclose(file);
        return;
    }

    uint8_t save_playback_data = 0;
    fread(&save_playback_data, 1, 1, file);

    if (!save_playback_data)
    {
        LOGERROR("Requested replay data but nothing was saved");
        fclose(file);
        return;
    }

    fread(&replay->count, sizeof(size_t), 1, file);
    fread(replay->frames, sizeof(replay_frame_t), replay->count, file);
    LOGDEBUG("loaded replay data with %ld input frames", replay->count);

    fclose(file);
}

void replay_dispose(replay_t* replay)
{
    if (replay == NULL)
        return;

    if (replay->frames)
        free(replay->frames);

    #ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll();
    #endif
}
