#include <logging.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define LOG_COLOR_COUNT     5
#define LOG_COLOR_NONE      "\033[0m"
#define LOG_COLOR_INFO      "\033[32m"
#define LOG_COLOR_DEBUG     "\033[34m"
#define LOG_COLOR_ERROR     "\033[31m"
#define LOG_COLOR_WARNING   "\033[33m"

static const char* LOG_COLORS[] = {
    LOG_COLOR_NONE,
    LOG_COLOR_INFO,
    LOG_COLOR_DEBUG,
    LOG_COLOR_ERROR,
    LOG_COLOR_WARNING,
};

// off by default
static uint8_t logging_debug_messages_enabled = 0;

// no output by default
static FILE* logging_log_messages_output_stream = NULL;

static const char* LOG_LEVEL_STRINGS[] = {
    "[ INFO ] \t",
    "[ DEBUG ]\t",
    "[ ERROR ]\t",
    "[WARNING]\t"
};

const char* logstringf(const char* formatString, ...)
{
    static char mFormattingBuffer[4096];

    va_list args;
    va_start(args, formatString);
    int result = vsnprintf(mFormattingBuffer, 4096, formatString, args);
    va_end(args);

    return mFormattingBuffer;
}

void log_enable_debug_messages(uint8_t val)
{
    logging_debug_messages_enabled = val;
}

void log_set_stream(FILE* stream)
{
    logging_log_messages_output_stream = stream;
}

void log_msg(loglevel_t lvl, const char* msg, const char* file, int line)
{
    if (lvl >= LOG_COLOR_COUNT - 1 || msg == NULL) return;
    if (logging_log_messages_output_stream == NULL) return;
    if (!logging_debug_messages_enabled && lvl == LOGLEVEL_DEBUG) return;

    time_t local_time = time(NULL);
    struct tm* tm = localtime(&local_time);

    fprintf(logging_log_messages_output_stream, "[%02d:%02d:%02d] %s(%s:%d): %s\n", tm->tm_hour, tm->tm_min, tm->tm_sec, LOG_LEVEL_STRINGS[lvl], file, line, msg);

    #ifndef __PSP__
    fprintf(stdout, "[%02d:%02d:%02d] %s(%s:%d): %s\n", tm->tm_hour, tm->tm_min, tm->tm_sec, LOG_LEVEL_STRINGS[lvl], file, line, msg);
    #endif

    if (logging_log_messages_output_stream != stdout && logging_log_messages_output_stream != stderr)
        fflush(logging_log_messages_output_stream);
}
