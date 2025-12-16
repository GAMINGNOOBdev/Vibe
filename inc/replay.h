#ifndef __REPLAY_H_
#define __REPLAY_H_

#include <scoring.h>
#include <stdlib.h>

typedef struct
{
    uint32_t key;
    uint8_t action;
    float timestamp;
} replay_frame_t;

typedef struct
{
    replay_frame_t* frames;
    size_t buffer_size;
    size_t cursor;
    size_t count;
} replay_t;

/**
 * @brief Initialize an empty replay
 */
void replay_initialize(replay_t* replay);

/**
 * @brief Clear current replay data
 */
void replay_clear(replay_t* replay);

/**
 * @brief Record inputs to replay
 * 
 * @param replay Replay
 * @param timestamp Current audio timestamp
 */
void replay_record_inputs(replay_t* replay, float timestamp);

/**
 * @brief Play back a replay (disables user input temporarily)
 * 
 * @param replay Replay
 * @param timestamp Current audio timestamp
 */
void replay_playback(replay_t* replay, float timestamp);

/**
 * @brief Save a replay to a file
 * 
 * @param replay Replay
 * @param score Score
 * @param save_playback_data Whether or not to save the actual playback data (replay can be set to NULL if 0)
 * @param filepath Path to target replay file
 */
void replay_save(replay_t* replay, score_t* score, uint8_t save_playback_data, const char* filepath);

/**
 * @brief Load a replay (with the score)
 * 
 * @param replay Replay (set to null if irrelevant)
 * @param score Score
 * @param filepath Path to replay
 */
void replay_load(replay_t* replay, score_t* score, const char* filepath);

/**
 * @brief Disposes a replay object
 * 
 * @param replay Replay
 */
void replay_dispose(replay_t* replay);

//////////////////////////
///                    ///
///   Replay manager   ///
///                    ///
//////////////////////////

#ifdef __PSP__
#   define REPLAY_MANAGER_MAX_COUNT 0x80
#else
#   define REPLAY_MANAGER_MAX_COUNT 0x100
#endif

typedef struct
{
    replay_t replays[REPLAY_MANAGER_MAX_COUNT];
    score_t scores[REPLAY_MANAGER_MAX_COUNT];
    uint16_t count;
} replay_manager_return_result_t;

/**
 * @brief Search for replays of a map inside a set
 *
 * @param set Beatmap set ID
 * @param map Beatmap ID
 * @param count Amount of replays that are found
 *
 * @returns The replays inside a buffer (pointer to a stack allocated array, do not free the returned address)
 */
replay_manager_return_result_t* replay_manager_search_for_map(uint64_t set, uint64_t map);

#endif
