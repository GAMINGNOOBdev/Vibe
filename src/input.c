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

int wait_for_input(void)
{
    #ifdef __PSP__
    for (int i = 0; i < 32; i++)
    {
        int button = 1 << i;
        if (button_pressed_once(button))
            return button;
    }
    #else
    for (size_t i = 0; i < SDLK_ENDCALL; i++)
    {
        if (button_pressed_once(i))
            return i;
    }
    #endif
    return -1;
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

const char* get_psp_button_string(int button)
{
    if (button == PSP_CTRL_SELECT)
        return "SELECT";
    if (button == PSP_CTRL_START)
        return "START";
    if (button == PSP_CTRL_UP)
        return "\x18";
    if (button == PSP_CTRL_RIGHT)
        return "\x1A";
    if (button == PSP_CTRL_DOWN)
        return "\x19";
    if (button == PSP_CTRL_LEFT)
        return "\x1B";
    if (button == PSP_CTRL_LTRIGGER)
        return "L";
    if (button == PSP_CTRL_RTRIGGER)
        return "R";
    if (button == PSP_CTRL_TRIANGLE)
        return "\x1E";
    if (button == PSP_CTRL_CIRCLE)
        return "O";
    if (button == PSP_CTRL_CROSS)
        return "X";
    if (button == PSP_CTRL_SQUARE)
        return "\xA";

    return "None";
}
