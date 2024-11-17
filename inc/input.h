#ifndef __INPUT_H_
#define __INPUT_H_ 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enables input
 * @note To use analog stick as well use PSP_CTRL_MODE_ANALOG mode
 */
void inputEnable(int mode);

/**
 * @brief Read the input buffer
 */
void inputRead();

uint8_t analogX();
uint8_t analogY();

/**
 * @brief Check if a button is pressed
 */
uint8_t buttonPressed(unsigned int button);

/**
 * @brief Check if a button is held
 */
uint8_t buttonHeld(unsigned int button);

#ifdef __cplusplus
}
#endif

#endif