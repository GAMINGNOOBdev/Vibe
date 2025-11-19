#ifndef __INPUT_H_
#define __INPUT_H_ 1

#ifdef __PSP__
#include <pspctrl.h>
#endif

#include <stdint.h>

/**
 * @brief Enables input
 * @note To use analog stick as well use PSP_CTRL_MODE_ANALOG mode
 */
void input_enable(int mode);

/**
 * @brief Lock inputs (used for replays)
 * 
 * @param value true/1 if locked, false/0 if not
 */
void input_lock(uint8_t value);

/**
 * @brief Send a custom button input
 * 
 * @param action Button action
 * @param button Button
 */
void input_send_button(uint8_t action, int button);

/**
 * @brief Read the input buffer
 */
void input_read();

#ifndef __PSP__
/**
 * @brief Write a button's state
 * 
 * @param key Key
 * @param value Value
 */
void input_write(int key, int value);
#endif

uint8_t analog_x();
uint8_t analog_y();

/**
 * @brief Waits for input and returns the last newly pressed button
 * 
 * @returns The newly pressed button
 */
int wait_for_input(void);

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
 * @brief Get the string representation of the psp buttons
 * 
 * @param button PSP Button
 */
const char* get_psp_button_string(int button);

#endif
