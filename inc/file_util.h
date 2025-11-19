#ifndef __FILE_UTIL_H_
#define __FILE_UTIL_H_ 1

#include <stddef.h>
#include <strutil.h>

#define FilterMaskFiles                 0x00 // Only files inside the given directory
#define FilterMaskAllFiles              0x01 // All files including files from subfolders
#define FilterMaskFolders               0x02 // Only folder inside the given directory
#define FilterMaskAllFolders            0x03 // All folders including folders from subdirectories
#define FilterMaskFilesAndFolders       0x04 // Only files and folders inside the given directory
#define FilterMaskAllFilesAndFolders    0x05 // All files and folders including those in subdirectories

/**
 * Calculates the filesize of a given file
 * 
 * @param filename Path to the file
 * @returns Size of the file on the disk
*/
size_t file_util_file_size(const char* filename);

/**
 * Get the contents of a given file
 * 
 * @note Resulting pointer should be free-d by the user
 * 
 * @param filename Path to the file
 * @returns Contents of the file on the disk
*/
void* file_util_file_contents(const char* filename);

/**
 * Gets the contents of the given directory
 * @note This function may take a while to complete since it will retrieve all files from subfolders as well
 * 
 * @param path Path to the directory
 * @param mask A filter which decides how a directories' contents shall be gotten
 * 
 * @returns A list of files inside the given directory
*/
stringvec_t file_util_get_directory_contents(const char* path, int mask);

/**
 * @brief Check whether a directory exists or not
 */
uint8_t file_util_directory_exists(const char* path);

/**
 * @brief Create a directory
 */
void file_util_create_directory(const char* path);

/**
 * Gets the file name (with extension) without the leading path
 * @param filePath File path
 * @returns The file name without the prepending path
*/
const char* file_util_get_file_name(const char* filePath);

/**
 * Gets the file extension from a file path
 * @param filePath File path
 * @returns Only the extension from the file path (WITH a `.`)
*/
const char* file_util_get_extension(const char* filePath);

#endif
