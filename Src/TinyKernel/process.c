/* =========================================================================
 * process.c — TinyVM Process Manager
 *
 * Manages a fixed-size process table. Currently launches native host
 * executables (.exe on Windows). The launch logic is isolated in
 * process_launch() so it can be swapped for the TinyVM loader later.
 * ========================================================================= */

#include "process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Platform/fileio.h"

/* Platform-specific headers for process launching. */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


/* =========================================================================
 * Internal state
 * ========================================================================= */

/** The process table — a fixed array of PCBs. */
static ProcessControlBlock process_table[PROCESS_TABLE_SIZE];

/** Monotonically increasing PID counter. */
static uint32_t next_pid = 1;

/** Whether process_init() has been called. */
static bool initialized = false;


/* =========================================================================
 * Internal helpers
 * ========================================================================= */

/**
 * Finds a free slot in the process table.
 * A slot is free if its PID is PID_INVALID or its state is PROCESS_TERMINATED.
 *
 * @return Index of the free slot, or -1 if the table is full.
 */
static int find_free_slot(void)
{
    for (int i = 0; i < PROCESS_TABLE_SIZE; i++) {
        if (process_table[i].pid == PID_INVALID ||
            process_table[i].state == PROCESS_TERMINATED) {
            return i;
        }
    }
    return -1;
}

/**
 * Finds the table index for a given PID.
 *
 * @return Index of the slot, or -1 if not found.
 */
static int find_slot_by_pid(uint32_t pid)
{
    for (int i = 0; i < PROCESS_TABLE_SIZE; i++) {
        if (process_table[i].pid == pid &&
            process_table[i].state != PROCESS_TERMINATED) {
            return i;
        }
    }
    return -1;
}

/**
 * Extracts the filename (without directory) from a path.
 * Used to populate the process name field.
 */
static const char* extract_filename(const char* path)
{
    const char* last_slash = path;

    for (const char* p = path; *p; p++) {
        if (*p == '/' || *p == '\\') {
            last_slash = p + 1;
        }
    }
    return last_slash;
}

/**
 * Checks whether a path ends with the given extension (case-insensitive).
 */
static bool has_extension(const char* path, const char* ext)
{
    size_t path_len = strlen(path);
    size_t ext_len  = strlen(ext);

    if (path_len < ext_len) return false;

    const char* suffix = path + path_len - ext_len;
    for (size_t i = 0; i < ext_len; i++) {
        char a = suffix[i];
        char b = ext[i];
        /* Lowercase ASCII for comparison. */
        if (a >= 'A' && a <= 'Z') a += 32;
        if (b >= 'A' && b <= 'Z') b += 32;
        if (a != b) return false;
    }
    return true;
}


/* =========================================================================
 * Platform-specific launch / kill / poll
 *
 * All host-OS interaction is isolated here. When TinyVM gets its own
 * executable format, only process_launch() needs to change.
 * ========================================================================= */

#ifdef _WIN32

/**
 * Windows implementation — launches a .exe using CreateProcessA.
 * Stores the PROCESS_INFORMATION block as the host handle.
 */
static bool process_launch(ProcessControlBlock* pcb)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION* pi;

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    pi = (PROCESS_INFORMATION*)malloc(sizeof(PROCESS_INFORMATION));
    if (!pi) return false;
    memset(pi, 0, sizeof(*pi));

    /* CreateProcessA needs a mutable copy of the command line. */
    char cmd[PROCESS_PATH_MAX];
    strncpy(cmd, pcb->path, PROCESS_PATH_MAX - 1);
    cmd[PROCESS_PATH_MAX - 1] = '\0';

    BOOL ok = CreateProcessA(
        NULL,           /* Application name (NULL = use command line). */
        cmd,            /* Command line. */
        NULL,           /* Process security attributes. */
        NULL,           /* Thread security attributes. */
        FALSE,          /* Inherit handles. */
        0,              /* Creation flags. */
        NULL,           /* Environment. */
        NULL,           /* Current directory. */
        &si,            /* Startup info. */
        pi              /* Process information (output). */
    );

    if (!ok) {
        free(pi);
        return false;
    }

    pcb->host_handle = pi;
    pcb->host_pid    = (uint32_t)pi->dwProcessId;
    pcb->state       = PROCESS_RUNNING;
    return true;
}

/**
 * Windows implementation — terminates a running host process.
 */
static void process_terminate_host(ProcessControlBlock* pcb)
{
    if (!pcb->host_handle) return;

    PROCESS_INFORMATION* pi = (PROCESS_INFORMATION*)pcb->host_handle;
    TerminateProcess(pi->hProcess, 1);
    CloseHandle(pi->hProcess);
    CloseHandle(pi->hThread);
    free(pi);

    pcb->host_handle = NULL;
    pcb->state       = PROCESS_TERMINATED;
    pcb->exit_code   = 1;
}

/**
 * Windows implementation — checks if a host process has exited.
 * If it has, stores the exit code and marks the PCB as terminated.
 */
static void process_poll_host(ProcessControlBlock* pcb)
{
    if (!pcb->host_handle) return;

    PROCESS_INFORMATION* pi = (PROCESS_INFORMATION*)pcb->host_handle;
    DWORD exit_code = 0;

    if (GetExitCodeProcess(pi->hProcess, &exit_code)) {
        if (exit_code != STILL_ACTIVE) {
            pcb->exit_code = (int)exit_code;
            pcb->state     = PROCESS_TERMINATED;

            CloseHandle(pi->hProcess);
            CloseHandle(pi->hThread);
            free(pi);

            pcb->host_handle = NULL;
        }
    }
}

#else /* Linux / macOS */

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/**
 * Linux/macOS implementation — launches a native executable using fork/exec.
 * Stores the child PID as the host_pid.
 */
static bool process_launch(ProcessControlBlock* pcb)
{
    pid_t child = fork();

    if (child < 0) {
        /* fork() failed. */
        return false;
    }

    if (child == 0) {
        /* Child process — execute the target. */
        execl(pcb->path, pcb->path, (char*)NULL);
        /* If execl returns, it failed. */
        _exit(127);
    }

    /* Parent process — store the child PID. */
    pcb->host_pid    = (uint32_t)child;
    pcb->host_handle = NULL; /* Not needed on POSIX. */
    pcb->state       = PROCESS_RUNNING;
    return true;
}

/**
 * Linux/macOS implementation — terminates a running host process.
 */
static void process_terminate_host(ProcessControlBlock* pcb)
{
    if (pcb->host_pid == 0) return;

    kill((pid_t)pcb->host_pid, SIGKILL);
    waitpid((pid_t)pcb->host_pid, NULL, 0);

    pcb->host_pid  = 0;
    pcb->state     = PROCESS_TERMINATED;
    pcb->exit_code = 1;
}

/**
 * Linux/macOS implementation — checks if a host process has exited.
 */
static void process_poll_host(ProcessControlBlock* pcb)
{
    if (pcb->host_pid == 0) return;

    int status = 0;
    pid_t result = waitpid((pid_t)pcb->host_pid, &status, WNOHANG);

    if (result > 0) {
        /* Child has exited. */
        if (WIFEXITED(status)) {
            pcb->exit_code = WEXITSTATUS(status);
        } else {
            pcb->exit_code = 1;
        }
        pcb->state    = PROCESS_TERMINATED;
        pcb->host_pid = 0;
    }
}

#endif /* _WIN32 */


/* =========================================================================
 * Public API
 * ========================================================================= */

void process_init(void)
{
    for (int i = 0; i < PROCESS_TABLE_SIZE; i++) {
        memset(&process_table[i], 0, sizeof(ProcessControlBlock));
        process_table[i].pid   = PID_INVALID;
        process_table[i].state = PROCESS_TERMINATED;
    }
    next_pid    = 1;
    initialized = true;
}

uint32_t process_create(const char* executable)
{
    if (!initialized) {
        fprintf(stderr, "[process] Error: process_init() has not been called.\n");
        return PID_INVALID;
    }

    if (!executable) {
        fprintf(stderr, "[process] Error: executable path is NULL.\n");
        return PID_INVALID;
    }

    /* Validate that the file exists. */
    if (!fileio_exists(executable)) {
        fprintf(stderr, "[process] Error: executable not found: %s\n", executable);
        return PID_INVALID;
    }

    /* Validate extension — on Windows, only .exe is supported for now. */
#ifdef _WIN32
    if (!has_extension(executable, ".exe")) {
        fprintf(stderr, "[process] Error: unsupported executable format: %s\n", executable);
        return PID_INVALID;
    }
#endif

    /* Find a free slot in the process table. */
    int slot = find_free_slot();
    if (slot < 0) {
        fprintf(stderr, "[process] Error: process table is full.\n");
        return PID_INVALID;
    }

    /* Initialize the PCB. */
    ProcessControlBlock* pcb = &process_table[slot];
    memset(pcb, 0, sizeof(ProcessControlBlock));

    pcb->pid = next_pid++;
    pcb->state = PROCESS_READY;
    pcb->exit_code = -1;

    strncpy(pcb->path, executable, PROCESS_PATH_MAX - 1);
    pcb->path[PROCESS_PATH_MAX - 1] = '\0';

    const char* filename = extract_filename(executable);
    strncpy(pcb->name, filename, PROCESS_NAME_MAX - 1);
    pcb->name[PROCESS_NAME_MAX - 1] = '\0';

    /* Attempt to launch the process on the host OS. */
    if (!process_launch(pcb)) {
        fprintf(stderr, "[process] Error: failed to launch: %s\n", executable);
        pcb->pid   = PID_INVALID;
        pcb->state = PROCESS_TERMINATED;
        return PID_INVALID;
    }

    return pcb->pid;
}

bool process_kill(uint32_t pid)
{
    int slot = find_slot_by_pid(pid);
    if (slot < 0) return false;

    ProcessControlBlock* pcb = &process_table[slot];
    process_terminate_host(pcb);
    pcb->state = PROCESS_TERMINATED;
    return true;
}

bool process_exists(uint32_t pid)
{
    return find_slot_by_pid(pid) >= 0;
}

ProcessControlBlock* process_get(uint32_t pid)
{
    int slot = find_slot_by_pid(pid);
    if (slot < 0) return NULL;
    return &process_table[slot];
}

void process_update(void)
{
    for (int i = 0; i < PROCESS_TABLE_SIZE; i++) {
        if (process_table[i].state == PROCESS_RUNNING) {
            process_poll_host(&process_table[i]);
        }
    }
}

void process_shutdown(void)
{
    /* Kill all processes that are still running. */
    for (int i = 0; i < PROCESS_TABLE_SIZE; i++) {
        if (process_table[i].state == PROCESS_RUNNING ||
            process_table[i].state == PROCESS_READY   ||
            process_table[i].state == PROCESS_SLEEPING) {
            process_terminate_host(&process_table[i]);
        }
        process_table[i].pid   = PID_INVALID;
        process_table[i].state = PROCESS_TERMINATED;
    }
    initialized = false;
}
