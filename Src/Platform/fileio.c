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

bool fileio_delete(const char* filepath) {
    if (!filepath) return false;
    return remove(filepath) == 0;
}

bool fileio_rename(const char* src, const char* dst) {
    if (!src || !dst) return false;
    return rename(src, dst) == 0;
}

bool fileio_copy(const char* src, const char* dst) {
    if (!src || !dst) return false;

    FILE* fsrc = fopen(src, "rb");
    if (!fsrc) return false;

    FILE* fdst = fopen(dst, "wb");
    if (!fdst) { fclose(fsrc); return false; }

    char buf[4096];
    size_t n;
    bool ok = true;
    while ((n = fread(buf, 1, sizeof(buf), fsrc)) > 0) {
        if (fwrite(buf, 1, n, fdst) != n) { ok = false; break; }
    }
    fclose(fsrc);
    fclose(fdst);
    if (!ok) remove(dst);
    return ok;
}

#ifdef _WIN32
#include <windows.h>
char** fileio_list_dir(const char* dirpath, size_t* out_count) {
    if (!dirpath || !out_count) return NULL;
    *out_count = 0;

    char pattern[512];
    snprintf(pattern, sizeof(pattern), "%s\\*", dirpath);

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return NULL;

    /* First pass: count */
    size_t count = 0;
    do {
        if (strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0)
            ++count;
    } while (FindNextFileA(h, &fd));
    FindClose(h);

    char** list = (char**)malloc(count * sizeof(char*));
    if (!list) return NULL;

    h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) { free(list); return NULL; }

    size_t i = 0;
    do {
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) continue;
        list[i++] = strdup(fd.cFileName);
    } while (FindNextFileA(h, &fd) && i < count);
    FindClose(h);

    *out_count = i;
    return list;
}
#else
#include <dirent.h>
#include <string.h>
char** fileio_list_dir(const char* dirpath, size_t* out_count) {
    if (!dirpath || !out_count) return NULL;
    *out_count = 0;

    DIR* d = opendir(dirpath);
    if (!d) return NULL;

    /* First pass: count entries */
    size_t count = 0;
    struct dirent* ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
            ++count;
    }
    rewinddir(d);

    char** list = (char**)malloc(count * sizeof(char*));
    if (!list) { closedir(d); return NULL; }

    size_t i = 0;
    while ((ent = readdir(d)) != NULL && i < count) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        list[i++] = strdup(ent->d_name);
    }
    closedir(d);
    *out_count = i;
    return list;
}
#endif

void fileio_list_free(char** list, size_t count) {
    if (!list) return;
    for (size_t i = 0; i < count; ++i) free(list[i]);
    free(list);
}

bool fileio_is_dir(const char* path) {
    if (!path) return false;
#ifdef _WIN32
    DWORD attr = GetFileAttributesA(path);
    return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat st;
    if (stat(path, &st) != 0) return false;
    return S_ISDIR(st.st_mode);
#endif
}
