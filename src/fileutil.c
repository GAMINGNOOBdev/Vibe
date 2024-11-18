#include <fileutil.h>
#include <logging.h>
#include <dirent.h>
#include <memory.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

#define MAX_OF(a, b) a > b ? a : b

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

stringvec_t* createStringVec()
{
    stringvec_t* vec = (stringvec_t*)malloc(sizeof(stringvec_t));
    memset(vec, 0, sizeof(stringvec_t));
    return vec;
}

void disposeStringVec(stringvec_t* vec)
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

    free(vec);
}

void pushBackStringVec(stringvec_t* vec, const char* value)
{
    if (vec == NULL || value == NULL)
        return;

    struct stringvec_entry* entry = malloc(sizeof(struct stringvec_entry));
    memset(entry, 0, sizeof(struct stringvec_entry));
    if (entry == NULL)
        return;

    size_t contentsSize = strlen(value)+1;
    entry->contents = malloc(contentsSize);
    memset(entry->contents, 0, contentsSize);
    memcpy(entry->contents, value, contentsSize-1);

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

size_t fileUtilFileSize(const char* filename)
{
    size_t result = 0;

    FILE* file = fopen(filename, "rb");
    
    fseek(file, 0, SEEK_END);
    result = ftell(file);
    rewind(file);

    fclose(file);

    return result;
}

void fileUtilConcatPathVectors(stringvec_t* output, stringvec_t* input, const char* prefix)
{
    if (output == NULL || input == NULL || prefix == NULL)
        return;

    if (input->count == 0)
        return;

    for (struct stringvec_entry* entry = input->start; entry != NULL; entry = entry->next)
    {
        const char* string = entry->contents;
        pushBackStringVec(output, stringf("%s%s", prefix, string));
    }
}

stringvec_t* fileUtilGetDirectoryContents(const char* path, int mask)
{
    stringvec_t* result = createStringVec();
    if (result == NULL)
        return NULL;

    DIR* directory = opendir(path);
    if (directory == NULL)
    {
        disposeStringVec(result);
        return result;
    }

    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        if (mask == FilterMaskAllFilesAndFolders)
        {
            if (entry->d_type == DT_DIR)
            {
                stringvec_t* subResult = fileUtilGetDirectoryContents(stringf("%s/%s", path, entry->d_name), mask);
                if (subResult != NULL)
                    fileUtilConcatPathVectors(result, subResult, stringf("%s/", entry->d_name));
                
                disposeStringVec(subResult);
            }

            pushBackStringVec(result, entry->d_name);
            continue;
        }

        if (mask == FilterMaskFilesAndFolders)
        {
            pushBackStringVec(result, entry->d_name);
            continue;
        }

        if (mask == FilterMaskFiles && entry->d_type != DT_DIR)
        {
            pushBackStringVec(result, entry->d_name);
            continue;
        }

        if (mask == FilterMaskFolders && entry->d_type == DT_DIR)
        {
            pushBackStringVec(result, entry->d_name);
            continue;
        }

        if (mask == FilterMaskAllFiles)
        {
            if (entry->d_type == DT_DIR)
            {
                stringvec_t* subResult = fileUtilGetDirectoryContents(stringf("%s/%s", path, entry->d_name), mask);
                if (subResult != NULL)
                    fileUtilConcatPathVectors(result, subResult, stringf("%s/", entry->d_name));
                
                disposeStringVec(subResult);
            }
            else
                pushBackStringVec(result, entry->d_name);
            
            continue;
        }

        if (mask == FilterMaskAllFolders)
        {
            if (entry->d_type == DT_DIR)
            {
                stringvec_t* subResult = fileUtilGetDirectoryContents(stringf("%s/%s", path, entry->d_name), mask);
                if (subResult != NULL)
                    fileUtilConcatPathVectors(result, subResult, stringf("%s/", entry->d_name));

                pushBackStringVec(result, entry->d_name);
            }

            continue;
        }
    }

    closedir(directory);

    return result;
}

const char* fileUtilGetFileName(const char* str)
{
    char delimiter0 = '/';
    char delimiter1 = '\\';

    if (strpos(str, delimiter0) == -1 && strpos(str, delimiter1) == -1)
        return str;

    int lastPathSeperator = MAX_OF((int64_t)strlpos(str, delimiter0), (int64_t)strlpos(str, delimiter1));

    if (lastPathSeperator == -1)
        return str;
    
    return (const char*)&str[lastPathSeperator+1];
}

const char* fileUtilGetExtension(const char* str)
{
    int lastDot = -1;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == '.')
            lastDot = i;
    }

    if (lastDot == -1)
        return str;
    
    return (const char*)&str[lastDot];
}