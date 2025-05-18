#include <time.h>
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

#include <psprtc.h>

double mDelta = 0.0;
double mElapsed = 0.0;
uint64_t mCurrTime = 0;
uint64_t mPastTime = 0;
uint32_t mFreq = 0;
uint32_t mFramesPerSecond = 0;

void time_init()
{
    mFreq = sceRtcGetTickResolution();
}

void time_tick()
{
    mPastTime = mCurrTime;
    while (1)
    {
        if (sceRtcGetCurrentTick(&mCurrTime) == 0)
            break;
    }

    mElapsed = (double)mCurrTime - (double)mPastTime;
    mDelta = mElapsed / (double)mFreq;
    mFramesPerSecond = (uint32_t)((double)mFreq / mElapsed);
}

double time_delta()
{
    return mDelta;
}

double time_elapsed()
{
    return mElapsed;
}

uint32_t time_fps()
{
    return mFramesPerSecond;
}