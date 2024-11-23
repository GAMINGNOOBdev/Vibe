#ifndef __SONGLIST_H_
#define __SONGLIST_H_ 1

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct songlist_entry_t;
struct songlist_entry_t
{
    struct songlist_entry_t* prev;
    uint64_t id;
    char* artist;
    char* songname;
    struct songlist_entry_t* next;
};

typedef struct
{
    struct songlist_entry_t* start;
    struct songlist_entry_t* end;
    size_t count;
} songlist_t;

songlist_t* createSongList(const char* path);
void disposeSongList(songlist_t* songlist);

#ifdef __cplusplus
}
#endif

#endif