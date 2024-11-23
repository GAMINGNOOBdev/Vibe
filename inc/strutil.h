#ifndef __STRUTIL_H_
#define __STRUTIL_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

int strpos(const char* str, char c);
int strlpos(const char* str, char c);
uint64_t strtoi(const char* str);

#ifdef __cplusplus
}
#endif

#endif