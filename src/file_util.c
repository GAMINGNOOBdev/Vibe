#include <file_util.h>
#include <logging.h>
#include <strutil.h>
#include <dirent.h>
#include <memory.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

#define MAX_OF(a, b) a > b ? a : b

size_t file_util_file_size(const char* filename)
{
    size_t result = 0;

    FILE* file = fopen(filename, "rb");
    if (file == NULL)
        return 0;
    
    fseek(file, 0, SEEK_END);
    result = ftell(file);
    rewind(file);

    fclose(file);

    return result;
}

void* file_util_file_contents(const char* filename)
{
    size_t size = 0;

    FILE* file = fopen(filename, "rb");
    if (file == NULL)
        return NULL;
    
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    void* result = malloc(size+1);
    memset(result, 0, size+1);
    fread(result, 1, size, file);

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
        stringvec_push_back(output, stringf("%s%s", prefix, string));
    }
}

stringvec_t file_util_get_directory_contents(const char* path, int mask)
{
    stringvec_t result;
    stringvec_initialize(result);
    DIR* directory = opendir(path);
    if (directory == NULL)
    {
        LOGERROR("could not find folder '%s'", path);
        stringvec_dispose(&result);
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
                stringvec_t subResult = file_util_get_directory_contents(stringf("%s/%s", path, entry->d_name), mask);
                if (subResult.start != NULL)
                    fileUtilConcatPathVectors(&result, &subResult, stringf("%s/", entry->d_name));
                
                stringvec_dispose(&subResult);
            }

            stringvec_push_back(&result, entry->d_name);
            continue;
        }

        if (mask == FilterMaskFilesAndFolders)
        {
            stringvec_push_back(&result, entry->d_name);
            continue;
        }

        if (mask == FilterMaskFiles && entry->d_type != DT_DIR)
        {
            stringvec_push_back(&result, entry->d_name);
            continue;
        }

        if (mask == FilterMaskFolders && entry->d_type == DT_DIR)
        {
            stringvec_push_back(&result, entry->d_name);
            continue;
        }

        if (mask == FilterMaskAllFiles)
        {
            if (entry->d_type == DT_DIR)
            {
                stringvec_t subResult = file_util_get_directory_contents(stringf("%s/%s", path, entry->d_name), mask);
                if (subResult.start != NULL)
                    fileUtilConcatPathVectors(&result, &subResult, stringf("%s/", entry->d_name));
                
                stringvec_dispose(&subResult);
            }
            else
                stringvec_push_back(&result, entry->d_name);
            
            continue;
        }

        if (mask == FilterMaskAllFolders)
        {
            if (entry->d_type == DT_DIR)
            {
                stringvec_t subResult = file_util_get_directory_contents(stringf("%s/%s", path, entry->d_name), mask);
                if (subResult.start != NULL)
                    fileUtilConcatPathVectors(&result, &subResult, stringf("%s/", entry->d_name));

                stringvec_push_back(&result, entry->d_name);
            }

            continue;
        }
    }

    closedir(directory);

    return result;
}

const char* file_util_get_file_name(const char* str)
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

const char* file_util_get_extension(const char* str)
{
    int lastDot = strlpos(str, '.');
    if (lastDot == -1)
        return str;
    
    return (const char*)&str[lastDot];
}
