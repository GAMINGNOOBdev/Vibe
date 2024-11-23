#include <logging.h>
#include <strutil.h>
#include <string.h>

int strpos(const char* str, char c)
{
    char* s = (char*)str;
    int idx = 0;
    while (*s != 0)
    {
        if (*s == c)
            return idx;
        s++;
        idx++;
    }
    return -1;
}

int strlpos(const char* str, char c)
{
    char* s = (char*)str;
    int idx = 0;
    int resIdx = -1;
    while (*s != 0)
    {
        if (*s == c)
            resIdx = idx;

        s++;
        idx++;
    }
    return resIdx;
}

uint64_t strtoi(const char* str)
{
    uint64_t res = 0;
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        res *= 10;
        res += str[i] - '0';
    }
    return res;
}