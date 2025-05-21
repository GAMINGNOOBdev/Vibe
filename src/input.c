#ifdef __PSP__
#include <pspctrl.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#endif
#include <input.h>
#include <logging.h>
#include <callback.h>

#ifdef __PSP__
SceCtrlData mInputData;
SceCtrlData mInputLastData;
#else
/// SDLK_ENDCALL is currently the last one i guess
uint8_t inputData[SDLK_ENDCALL];
uint8_t lastInputData[SDLK_ENDCALL];
#endif

void input_enable(int mode)
{
    #ifdef __PSP__
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(mode);
    #else
    if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
    {
        LOGERROR(stringf("SDL_InitSubSystem failed: %s", SDL_GetError()));
        return;
    }
    #endif
}

void input_read()
{
    #ifdef __PSP__
    mInputLastData = mInputData;
    sceCtrlReadBufferPositive(&mInputData,1);
    #else
    memcpy(lastInputData, inputData, SDLK_ENDCALL);

    SDL_Event event;
    while (SDL_PollEvent(&event))
        if (event.type == SDL_QUIT)
            stop_running();
        else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
            if (event.key.keysym.sym >= 0 && event.key.keysym.sym < SDLK_ENDCALL)
                inputData[event.key.keysym.sym] = event.key.state;
    #endif
}

uint8_t analog_x()
{
    #ifdef __PSP__
    return mInputData.Lx;
    #else
    //// TODO: --- add this back for desktop ---
    return 0;
    #endif
}

uint8_t analog_y()
{
    #ifdef __PSP__
    return mInputData.Ly;
    #else
    //// TODO: --- add this back for desktop ---
    return 0;
    #endif
}

uint8_t button_pressed(uint32_t button)
{
    #ifdef __PSP__
    return (mInputData.Buttons & button) ? 1 : 0;
    #else
    return inputData[button];
    #endif
}

uint8_t button_pressed_once(uint32_t button)
{
    #ifdef __PSP__
    return ((mInputData.Buttons & button) && !(mInputLastData.Buttons & button)) ? 1 : 0;
    #else
    return inputData[button] && !lastInputData[button];
    #endif
}

uint8_t button_held(uint32_t button)
{
    #ifdef __PSP__
    return ((mInputData.Buttons & button) && (mInputLastData.Buttons & button)) ? 1 : 0;
    #else
    return inputData[button] && lastInputData[button];
    #endif
}

uint8_t button_released(uint32_t button)
{
    #ifdef __PSP__
    return (!(mInputData.Buttons & button) && (mInputLastData.Buttons & button)) ? 1 : 0;
    #else
    return !inputData[button] && lastInputData[button];
    #endif
}