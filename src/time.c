#include <time.h>
#include <memory.h>
#include <sys/types.h>

///
/// These defines are for the psprtc.h header file because it requires them
///
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#ifdef __PSP__
#include <psprtc.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#endif

time_data_t time_data;

void time_init()
{
    #ifdef __PSP__
    time_data.freq = sceRtcGetTickResolution();
    #else
    time_data.freq = SDL_GetPerformanceFrequency();
    #endif
}

void time_tick()
{
    time_data.pastTime = time_data.currTime;
    #ifdef __PSP__
    while (1)
    {
        if (sceRtcGetCurrentTick(&time_data.currTime) == 0)
            break;
    }
    #else
    time_data.currTime = SDL_GetTicks64();
    #endif

    time_data.elapsed = (double)time_data.currTime - (double)time_data.pastTime;
    time_data.delta = time_data.elapsed / (double)time_data.freq;
    time_data.fps = (uint32_t)((double)time_data.freq / time_data.elapsed);
}

double time_delta()
{
    return time_data.delta;
}

double time_elapsed()
{
    return time_data.elapsed;
}

uint32_t time_fps()
{
    return time_data.fps;
}