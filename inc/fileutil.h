#ifndef __FILEUTIL_H_
#define __FILEUTIL_H_ 1

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FilterMaskFiles                 0x00 // Only files inside the given directory
#define FilterMaskAllFiles              0x01 // All files including files from subfolders
#define FilterMaskFolders               0x02 // Only folder inside the given directory
#define FilterMaskAllFolders            0x03 // All folders including folders from subdirectories
#define FilterMaskFilesAndFolders       0x04 // Only files and folders inside the given directory
#define FilterMaskAllFilesAndFolders    0x05 // All files and folders including those in subdirectories

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

stringvec_t* createStringVec();
void disposeStringVec(stringvec_t* vec);
void pushBackStringVec(stringvec_t* vec, const char* value);

/**
 * Calculates the filesize of a given file
 * 
 * @param filename Path to the file
 * @returns Size of the file on the disk
*/
size_t fileUtilFileSize(const char* filename);

/**
 * Gets the contents of the given directory
 * @note This function may take a while to complete since it will retrieve all files from subfolders as well
 * 
 * @param path Path to the directory
 * @param mask A filter which decides how a directories' contents shall be gotten
 * 
 * @returns A list of files inside the given directory
*/
stringvec_t* fileUtilGetDirectoryContents(const char* path, int mask);

/**
 * Gets the file name (with extension) without the leading path
 * @param filePath File path
 * @returns The file name without the prepending path
*/
const char* fileUtilGetFileName(const char* filePath);

/**
 * Gets the file extension from a file path
 * @param filePath File path
 * @returns Only the extension from the file path (WITH a `.`)
*/
const char* fileUtilGetExtension(const char* filePath);

#ifdef __cplusplus
}
#endif

#endif