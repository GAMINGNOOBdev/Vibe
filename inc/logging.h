#ifndef __BARANIUM__LOGGING_H_
#define __BARANIUM__LOGGING_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

typedef unsigned char loglevel_t;

#define LOGLEVEL_INFO       0
#define LOGLEVEL_DEBUG      1
#define LOGLEVEL_ERROR      2
#define LOGLEVEL_WARNING    3

#define LOG logStr
#define LOGINFO(msg) logStr(LOGLEVEL_INFO, msg)
#define LOGDEBUG(msg) logStr(LOGLEVEL_DEBUG, msg)
#define LOGERROR(msg) logStr(LOGLEVEL_ERROR, msg)
#define LOGWARNING(msg) logStr(LOGLEVEL_WARNING, msg)

/**
 * @brief Like printf but for building a string together
 * 
 * @param[in] formatString string which has format information
 * @param[in] ... any other arguments
 * 
 * @returns the new formatted string
*/
const char* stringf(const char* formatString, ...);

/**
 * @brief En-/Disable debug messages showing up
 * 
 * @note By default debug messages are off
 * 
 * @param val "Boolean" value, 1 = debug messages show up, 0 = no debug messages
 */
void logEnableDebugMsgs(uint8_t val);

/**
 * @brief Set an output stream for log messages
 * 
 * @param stream The output stream to which will be written, NULL to disable logging
 */
void logSetStream(FILE* stream);

/**
 * @brief Log a message onto the cmd line
 * 
 * @note Logging it turned off by default, set the output stream first before logging messages
 * 
 * @param lvl Logging level
 * @param msg Log message
 */
void logStr(loglevel_t lvl, const char* msg);

#ifdef __cplusplus
}
#endif

#endif