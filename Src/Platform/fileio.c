#include "fileio.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#define RMDIR(path) _rmdir(path)
#else
#include <sys/stat.h>
#include <unistd.h>
#define MKDIR(path) mkdir(path, 0777)
#define RMDIR(path) rmdir(path)
#endif
unsigned char* fileio_read(const char* filepath, size_t* out_size) {
    if (!filepath || !out_size) return NULL;
    
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        *out_size = 0;
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (size < 0) {
        fclose(file);
        *out_size = 0;
        return NULL;
    }
    
    unsigned char* buffer = (unsigned char*)malloc((size_t)size + 1);
    if (!buffer) {
        fclose(file);
        *out_size = 0;
        return NULL;
    }
    
    size_t read_size = fread(buffer, 1, (size_t)size, file);
    buffer[read_size] = '\0'; // Null-terminate for safety
    
    *out_size = read_size;
    fclose(file);
    
    return buffer;
}

bool fileio_write(const char* filepath, const void* data, size_t size) {
    if (!filepath || (!data && size > 0)) return false;
    
    FILE* file = fopen(filepath, "wb");
    if (!file) return false;
    
    size_t written = 0;
    if (size > 0) {
        written = fwrite(data, 1, size, file);
    }
    
    fclose(file);
    return written == size;
}

bool fileio_exists(const char* filepath) {
    if (!filepath) return false;
    
    FILE* file = fopen(filepath, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

size_t fileio_size(const char* filepath) {
    if (!filepath) return 0;
    
    FILE* file = fopen(filepath, "rb");
    if (!file) return 0;
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    
    return (size >= 0) ? (size_t)size : 0;
}

bool fileio_mkdir(const char* dirpath) {
    if (!dirpath) return false;
    return MKDIR(dirpath) == 0;
}

bool fileio_rmdir(const char* dirpath) {
    if (!dirpath) return false;
    return RMDIR(dirpath) == 0;
}
