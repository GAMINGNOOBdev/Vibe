#ifndef __LOGGING_H_
#define __LOGGING_H_ 1

#include <stdint.h>
#include <stdio.h>

typedef unsigned char loglevel_t;

#define LOGLEVEL_INFO       0
#define LOGLEVEL_DEBUG      1
#define LOGLEVEL_ERROR      2
#define LOGLEVEL_WARNING    3

#define LOG(level, ...) log_msg(level, logstringf(__VA_ARGS__), __FILE_NAME__, __LINE__)
#define LOGINFO(...) log_msg(LOGLEVEL_INFO, logstringf(__VA_ARGS__), __FILE_NAME__, __LINE__)
#define LOGDEBUG(...) log_msg(LOGLEVEL_DEBUG, logstringf(__VA_ARGS__), __FILE_NAME__, __LINE__)
#define LOGERROR(...) log_msg(LOGLEVEL_ERROR, logstringf(__VA_ARGS__), __FILE_NAME__, __LINE__)
#define LOGWARNING(...) log_msg(LOGLEVEL_WARNING, logstringf(__VA_ARGS__), __FILE_NAME__, __LINE__)

/**
 * @brief Like printf but for building a string together
 * 
 * @param[in] formatString string which has format information
 * @param[in] ... any other arguments
 * 
 * @returns the new formatted string
*/
const char* logstringf(const char* formatString, ...);

/**
 * @brief En-/Disable debug messages showing up
 * 
 * @note By default debug messages are off
 * 
 * @param val "Boolean" value, 1 = debug messages show up, 0 = no debug messages
 */
void log_enable_debug_messages(uint8_t val);

/**
 * @brief Set an output stream for log messages
 * 
 * @param stream The output stream to which will be written, NULL to disable logging
 */
void log_set_stream(FILE* stream);

/**
 * @brief Log a message onto the cmd line
 * 
 * @note Logging it turned off by default, set the output stream first before logging messages
 * 
 * @param lvl Logging level
 * @param msg Log message
 * @param file File
 * @param line Line
 */
void log_msg(loglevel_t lvl, const char* msg, const char* file, int line);

#endif
