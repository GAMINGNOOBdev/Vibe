#include <pspctrl.h>
#include <input.h>

SceCtrlData mInputData;
SceCtrlData mInputLastData;

void inputEnable(int mode)
{
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(mode);
}

void inputRead()
{
    mInputLastData = mInputData;
    sceCtrlReadBufferPositive(&mInputData,1);
}

SceCtrlData getInputData()
{
    return mInputData;
}

uint8_t analogX()
{
    return mInputData.Lx;
}

uint8_t analogY()
{
    return mInputData.Ly;
}

uint8_t buttonPressed(uint32_t button)
{
    if(mInputData.Buttons & button)
        return 1;

    return 0;
}

uint8_t buttonHeld(uint32_t button)
{
    if ((mInputData.Buttons & button) && (mInputLastData.Buttons & button))
        return 1;

    return 0;
}

uint8_t buttonReleased(uint32_t button)
{
    if (!(mInputData.Buttons & button) && (mInputLastData.Buttons & button))
        return 1;

    return 0;
}