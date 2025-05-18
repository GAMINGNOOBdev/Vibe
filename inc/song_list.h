#ifndef __SONG_LIST_H_
#define __SONG_LIST_H_ 1

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif



struct songlist_entry_t;
typedef struct songlist_entry_t
{
    struct songlist_entry_t* prev;
    uint64_t id;
    char* artist;
    char* songname;
    struct songlist_entry_t* next;
} songlist_entry_t;

typedef struct
{
    songlist_entry_t* start;
    songlist_entry_t* end;
    size_t count;
} songlist_t;

extern songlist_t songs_list;

void song_list_initialize(const char* path);
songlist_entry_t* songlist_get_entry(int index);
void song_list_dispose(void);

#ifdef __cplusplus
}
#endif

#endif