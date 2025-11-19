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

#endif
