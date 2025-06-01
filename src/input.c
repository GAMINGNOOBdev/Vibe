#include "time.h"
#ifdef __PSP__
#include <pspctrl.h>
#else
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif
#include <input.h>
#include <logging.h>
#include <callback.h>

#ifdef __PSP__
SceCtrlData mInputData;
SceCtrlData mInputLastData;
#else
#include <pctypes.h>
#include <memory.h>
int last_pressed_key = -1;
typedef struct
{
    int state;
    uint64_t pressed_frame;
} glfw_key_t;

glfw_key_t inputData[GLFW_KEY_LAST];

extern GLFWwindow* glfwwindow;
extern void graphicsWindowKeyboardEvent(GLFWwindow* win, int key, int scancode, int action, int _);
#endif

void input_enable(int mode)
{
    #ifdef __PSP__
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(mode);
    #endif
}

void input_read()
{
    #ifdef __PSP__
    mInputLastData = mInputData;
    sceCtrlReadBufferPositive(&mInputData,1);
    #else
    last_pressed_key = -1;
    for (int i = 0; i < GLFW_KEY_LAST; i++)
    {
        if (inputData[i].state == GLFW_PRESS && time_total_frames() > inputData[i].pressed_frame + 1)
            graphicsWindowKeyboardEvent(glfwwindow, i, -1, GLFW_REPEAT, -1);
    }
    #endif
}


#ifndef __PSP__
void input_write(int key, int value)
{
    inputData[key].state = value;
    inputData[key].pressed_frame = time_total_frames();

    /*LOGDEBUG(stringf("key,value: { '%s' | '%s' (0x%8.8x) }",
                     get_psp_button_string(key),
                     (value == GLFW_PRESS) ? "GLFW_PRESS" :
                     (value == GLFW_REPEAT) ? "GLFW_REPEAT" :
                     (value == GLFW_RELEASE) ? "GLFW_RELEASE" :
                     "UNKNOWN"));*/
}
#endif

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

    return -1;
    #else
    for (int i = 0; i < GLFW_KEY_LAST; i++)
    {
        if (button_pressed_once(i))
            return i;
    }

    return -1;
    #endif
}

uint8_t button_pressed(uint32_t button)
{
    #ifdef __PSP__
    return (mInputData.Buttons & button) ? 1 : 0;
    #else
    return inputData[button].state == GLFW_PRESS || inputData[button].state == GLFW_REPEAT;
    #endif
}

uint8_t button_pressed_once(uint32_t button)
{
    #ifdef __PSP__
    return ((mInputData.Buttons & button) && !(mInputLastData.Buttons & button)) ? 1 : 0;
    #else
    return inputData[button].state == GLFW_PRESS;
    #endif
}

uint8_t button_held(uint32_t button)
{
    #ifdef __PSP__
    return ((mInputData.Buttons & button) && (mInputLastData.Buttons & button)) ? 1 : 0;
    #else
    return inputData[button].state == GLFW_REPEAT;
    #endif
}

const char* get_psp_button_string(int button)
{
    #ifdef __PSP__
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
        return "0";
    if (button == PSP_CTRL_CROSS)
        return "X";
    if (button == PSP_CTRL_SQUARE)
        return "O";
    #else
    const char* name = glfwGetKeyName(button, glfwGetKeyScancode(button));
    if (name)
        return name;
    #endif

    return "None";
}
