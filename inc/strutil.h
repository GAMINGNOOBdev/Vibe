#ifndef __STRUTIL_H_
#define __STRUTIL_H_ 1

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Like printf but for building a string together
 * 
 * @param[in] formatString string which has format information
 * @param[in] ... any other arguments
 * 
 * @returns the new formatted string
*/
const char* stringf(const char* formatString, ...);

struct stringvec_entry;
struct stringvec_entry
{
    struct stringvec_entry* prev;
    char* contents;
    struct stringvec_entry* next;
};

typedef struct
{
    struct stringvec_entry* start;
    struct stringvec_entry* end;
    size_t count;
} stringvec_t;

#define stringvec_initialize(vec) memset(&vec, 0, sizeof(stringvec_t))
void stringvec_dispose(stringvec_t* vec);
void stringvec_push_back(stringvec_t* vec, const char* value);

int strpos(const char* str, char c);
int strlpos(const char* str, char c);
uint64_t strtoi(const char* str);

#endif
