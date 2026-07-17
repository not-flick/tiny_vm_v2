#ifndef FILEIO_H
#define FILEIO_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Reads an entire file into a newly allocated buffer.
 * The caller is responsible for freeing the returned buffer using free().
 * The buffer is null-terminated for safety if used as a string.
 *
 * @param filepath The path to the file to read.
 * @param out_size Pointer to store the size of the read data.
 * @return Pointer to the allocated buffer, or NULL on failure.
 */
unsigned char* fileio_read(const char* filepath, size_t* out_size);

/**
 * Writes data to a file, overwriting it if it already exists.
 *
 * @param filepath The path to the file to write.
 * @param data Pointer to the data to write.
 * @param size The number of bytes to write.
 * @return true on success, false on failure.
 */
bool fileio_write(const char* filepath, const void* data, size_t size);

/**
 * Checks if a file exists and is readable.
 *
 * @param filepath The path to the file.
 * @return true if the file exists, false otherwise.
 */
bool fileio_exists(const char* filepath);

/**
 * Gets the size of a file in bytes.
 *
 * @param filepath The path to the file.
 * @return The size of the file in bytes, or 0 on error.
 */
size_t fileio_size(const char* filepath);

/**
 * Creates a directory.
 *
 * @param dirpath The path of the directory to create.
 * @return true on success, false on failure.
 */
bool fileio_mkdir(const char* dirpath);

/**
 * Deletes an empty directory.
 *
 * @param dirpath The path of the directory to delete.
 * @return true on success, false on failure.
 */
bool fileio_rmdir(const char* dirpath);

#ifdef __cplusplus
}
#endif

#endif // FILEIO_H
