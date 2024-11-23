#include <songlist.h>
#include <fileutil.h>
#include <logging.h>
#include <strutil.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

void pushBackSongListEntry(songlist_t* vec, const char* value)
{
    if (vec == NULL || value == NULL)
        return;

    struct songlist_entry_t* entry = malloc(sizeof(struct songlist_entry_t));
    memset(entry, 0, sizeof(struct songlist_entry_t));
    if (entry == NULL)
        return;

    size_t length = strlen(value);
    size_t firstSpace = (size_t)strpos(value, ' ');
    size_t firstHyphen = (size_t)strpos(value, '-');
    size_t artistSize = firstHyphen - firstSpace - 2;
    size_t songnameSize = length - firstHyphen - 2;

    if (length == (size_t)-1 || firstSpace == (size_t)-1 || firstHyphen == (size_t)-1)
    {
        free(entry);
        LOGERROR("Unable to allocate song list entry: path length is undefined");
        return;
    }

    LOGDEBUG(stringf("value '%s', firstSpace = '%d', firstHyphen = '%d', len = '%ld'(0x%8.8x), artistSz = '%d', songNameSz = '%d'", value, firstSpace, firstHyphen, length, length, artistSize, songnameSize));

    entry->artist = malloc(artistSize+1);
    memset(entry->artist, 0, artistSize+1);
    memcpy(entry->artist, &value[firstSpace+1], artistSize);

    entry->songname = malloc(songnameSize+1);
    memset(entry->songname, 0, songnameSize+1);
    memcpy(entry->songname, &value[firstHyphen+2], songnameSize);

    char* idStr = malloc(firstSpace+1);
    memset(idStr, 0, firstSpace+1);
    memcpy(idStr, value, firstSpace);

    entry->id = strtoi(idStr);
    LOGDEBUG(stringf("id: %lld ('%s')", entry->id, idStr));
    free(idStr);

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

songlist_t* createSongList(const char* path)
{
    songlist_t* songlist = (songlist_t*)malloc(sizeof(songlist_t));
    memset(songlist, 0, sizeof(songlist_t));

    stringvec_t* songs = fileUtilGetDirectoryContents(path, FilterMaskFolders);

    for (struct stringvec_entry* entry = songs->start; entry != NULL; entry = entry->next)
        pushBackSongListEntry(songlist, entry->contents);

    disposeStringVec(songs);

    return songlist;
}

void disposeSongList(songlist_t* songlist)
{
    if (songlist == NULL)
        return;

    for (struct songlist_entry_t* entry = songlist->start; entry != NULL;)
    {
        struct songlist_entry_t* next = entry->next;
        free(entry->artist);
        free(entry->songname);
        free(entry);
        entry = next;
    }

    free(songlist);
}