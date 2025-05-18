#include <logging.h>
#include <strutil.h>
#include <string.h>
#include <stdlib.h>

void stringvec_dispose(stringvec_t* vec)
{
    if (vec == NULL)
        return;

    for (struct stringvec_entry* entry = vec->start; entry != NULL;)
    {
        struct stringvec_entry* next = entry->next;
        free(entry->contents);
        free(entry);
        entry = next;
    }
}

void stringvec_push_back(stringvec_t* vec, const char* value)
{
    if (vec == NULL || value == NULL)
        return;

    struct stringvec_entry* entry = malloc(sizeof(struct stringvec_entry));
    memset(entry, 0, sizeof(struct stringvec_entry));
    if (entry == NULL)
        return;

    size_t contentsSize = strlen(value);
    entry->contents = malloc(contentsSize+1);
    memset(entry->contents, 0, contentsSize+1);
    memcpy(entry->contents, value, contentsSize);

    if (vec->start == NULL)
    {
        vec->start = vec->end = entry;
        vec->count++;
        return;
    }

    entry->prev = vec->end;
    vec->end->next = entry;
    vec->end = entry;
    vec->count++;
}

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
