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

uint8_t analogX()
{
    return mInputData.Lx;
}

uint8_t analogY()
{
    return mInputData.Ly;
}

uint8_t buttonPrssed(unsigned int button)
{
    return mInputData.Buttons & button;
}

uint8_t buttonHeld(unsigned int button)
{
    return (mInputData.Buttons & button) && (mInputLastData.Buttons & button);
}