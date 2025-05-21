#include <file_util.h>
#include <song_list.h>
#include <logging.h>
#include <strutil.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

#ifdef __PSP__
#include <pspkernel.h>
#endif

songlist_t songs_list = {NULL, NULL, 0};

void song_list_add_entry(songlist_t* vec, const char* value, const char* parent_dir)
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

    entry->fullname = malloc(strlen(parent_dir)+length+2);
    memset(entry->fullname, 0, strlen(parent_dir)+length+2);
    memcpy(entry->fullname, parent_dir, strlen(parent_dir));
    entry->fullname[strlen(parent_dir)] = '/';
    memcpy(&entry->fullname[strlen(parent_dir)+1], value, length);

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
    LOGDEBUG(stringf("artist '%s', songname = '%s', fullname = '%s'", entry->artist, entry->songname, entry->fullname));

    stringvec_t songs = file_util_get_directory_contents(stringf("%s/%s", parent_dir, value), FilterMaskFiles);
    entry->difficulties.count = 0;
    for (struct stringvec_entry* file = songs.start; file != NULL; file = file->next)
    {
        int pos = strlpos(file->contents, '.');
        if (pos == -1)
            continue;

        if (strcmp(&file->contents[pos+1], "osu") == 0)
            entry->difficulties.count++;
    }

    if (entry->difficulties.count == 0)
    {
        free(entry);
        return;
    }

    entry->difficulties.data = malloc(sizeof(song_difficulty_t)*entry->difficulties.count);
    int difficultyIndex = 0;
    for (struct stringvec_entry* file = songs.start; file != NULL; file = file->next)
    {
        char* filename = file->contents;
        int pos = strlpos(filename, '.');
        if (pos == -1)
            continue;

        if (strcmp(&filename[pos+1], "osu") != 0)
            continue;

        song_difficulty_t* diff = &entry->difficulties.data[difficultyIndex];
        const char* name = stringf("%s/%s/%s", parent_dir, value, filename);
        diff->filename = malloc(strlen(name)+1);
        strcpy(diff->filename, name);

        int bracketOpen = strpos(filename, '[');
        int bracketClose = strlpos(filename, ']');
        int parenthesisOpen = strlpos(filename, '(');
        int parenthesisClose = strlpos(filename, ')');

        diff->mapper = malloc(parenthesisClose-parenthesisOpen);
        diff->name = malloc(bracketClose-bracketOpen);
        memset(diff->mapper, 0, parenthesisClose-parenthesisOpen);
        memset(diff->name, 0, bracketClose-bracketOpen);
        strncpy(diff->mapper, &filename[parenthesisOpen+1], parenthesisClose-parenthesisOpen-1);
        strncpy(diff->name, &filename[bracketOpen+1], bracketClose-bracketOpen-1);

        difficultyIndex++;
    }

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

void song_list_initialize(const char* path)
{
    song_list_dispose();
    memset(&songs_list, 0, sizeof(songlist_t));

    stringvec_t songs = file_util_get_directory_contents(path, FilterMaskFolders);

    for (struct stringvec_entry* entry = songs.start; entry != NULL; entry = entry->next)
        song_list_add_entry(&songs_list, entry->contents, path);

    #ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll();
    #endif

    stringvec_dispose(&songs);
}

songlist_entry_t* songlist_get_entry(int index)
{
    songlist_entry_t* entry = songs_list.start;
    int idx = 0;
    while (entry && idx < index)
    {
        entry = entry->next;
        idx++;
    }
    return entry;
}

void song_list_dispose(void)
{
    for (struct songlist_entry_t* entry = songs_list.start; entry != NULL;)
    {
        struct songlist_entry_t* next = entry->next;
        for (int i = 0; i < entry->difficulties.count; i++)
        {
            free(entry->difficulties.data[i].filename);
            free(entry->difficulties.data[i].mapper);
            free(entry->difficulties.data[i].name);
        }
        free(entry->difficulties.data);
        free(entry->artist);
        free(entry->songname);
        free(entry);
        entry = next;
    }
}
