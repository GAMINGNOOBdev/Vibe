#ifndef __INPUT_H_
#define __INPUT_H_ 1

#include <pspctrl.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enables input
 * @note To use analog stick as well use PSP_CTRL_MODE_ANALOG mode
 */
void input_enable(int mode);

/**
 * @brief Read the input buffer
 */
void input_read();

SceCtrlData input_get_data();

uint8_t analog_x();
uint8_t analog_y();

/**
 * @brief Check if a button is pressed down (not only once!)
 */
uint8_t button_pressed(uint32_t button);

/**
 * @brief Check if a button is pressed only once
 */
uint8_t button_pressed_once(uint32_t button);

/**
 * @brief Check if a button is held
 */
uint8_t button_held(uint32_t button);

/**
 * @brief Check if a button is released
 */
uint8_t button_released(uint32_t button);

#ifdef __cplusplus
}
#endif

#endif
