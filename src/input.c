#include <pspctrl.h>
#include <input.h>

SceCtrlData mInputData;
SceCtrlData mInputLastData;

void input_enable(int mode)
{
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(mode);
}

void input_read()
{
    mInputLastData = mInputData;
    sceCtrlReadBufferPositive(&mInputData,1);
}

SceCtrlData input_get_data()
{
    return mInputData;
}

uint8_t analog_x()
{
    return mInputData.Lx;
}

uint8_t analog_y()
{
    return mInputData.Ly;
}

uint8_t button_pressed(uint32_t button)
{
    if(mInputData.Buttons & button)
        return 1;

    return 0;
}

uint8_t button_pressed_once(uint32_t button)
{
    if((mInputData.Buttons & button) && !(mInputLastData.Buttons & button))
        return 1;

    return 0;
}

uint8_t button_held(uint32_t button)
{
    if ((mInputData.Buttons & button) && (mInputLastData.Buttons & button))
        return 1;

    return 0;
}

uint8_t button_released(uint32_t button)
{
    if (!(mInputData.Buttons & button) && (mInputLastData.Buttons & button))
        return 1;

    return 0;
}