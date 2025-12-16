#include <file_util.h>
#include <stdint.h>
#include <strutil.h>
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
    if (replay == NULL || replay->count == 0 || timestamp <= 0)
        return;

    if (replay->cursor == replay->count) // end of replay
        return;

    replay_frame_t frame = replay->frames[replay->cursor];
    if (timestamp >= frame.timestamp)
    {
        input_write(frame.key, frame.action);
        replay->cursor++;
        replay_playback(replay, timestamp); // handle all other inputs that may be valid
    }
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
    if (replay->count != 0)
    {
        replay->buffer_size = (size_t)(replay->count / REPLAY_BUFFER_SIZE + 1) * REPLAY_BUFFER_SIZE;
        replay->frames = realloc(replay->frames, replay->buffer_size * sizeof(replay_frame_t));
    }
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

//////////////////////////
///                    ///
///   Replay manager   ///
///                    ///
//////////////////////////

replay_manager_return_result_t replay_manager_return_result = {0,0,0};

void replay_manager_file_iterator_callback(const char* _, const char* filename, void* userdata)
{
    struct { uint64_t map, set; }* data = userdata;
    const char* filename_start = stringf("%lld_%lld", data->set, data->map);
    if (strlen(filename) < strlen(filename_start))
        return;
    if (strncmp(filename, filename_start, strlen(filename_start)) != 0)
        return;

    uint16_t* count = &replay_manager_return_result.count;
    replay_t* replay = &replay_manager_return_result.replays[*count];
    score_t* score = &replay_manager_return_result.scores[*count];
    replay_load(replay, score, stringf("Replays/%s", filename));
    (*count)++;
}

replay_manager_return_result_t* replay_manager_search_for_map(uint64_t set, uint64_t map)
{
    for (size_t i = 0; i < replay_manager_return_result.count; i++)
        replay_dispose(&replay_manager_return_result.replays[i]);

    memset(&replay_manager_return_result, 0, sizeof(replay_manager_return_result_t));

    if (!file_util_directory_exists("Replays"))
    {
        LOGERROR("Cannot find any replays");
        return NULL;
    }
    struct {
        uint64_t map, set;
    } data = {map, set};

    file_util_iterate_directory("Replays", FilterMaskFiles, replay_manager_file_iterator_callback, &data);

    return &replay_manager_return_result;
}

