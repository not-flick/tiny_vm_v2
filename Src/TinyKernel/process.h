#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Constants
 * ========================================================================= */

/** Maximum number of concurrent processes. */
#define PROCESS_TABLE_SIZE 256

/** Maximum length for a process name (including null terminator). */
#define PROCESS_NAME_MAX 128

/** Maximum length for an executable path (including null terminator). */
#define PROCESS_PATH_MAX 512

/** Number of reserved registers for future TinyVM CPU context. */
#define PROCESS_REGISTER_COUNT 32

/** Invalid PID sentinel value. */
#define PID_INVALID ((uint32_t)-1)


/* =========================================================================
 * Process States
 * ========================================================================= */

typedef enum {
    PROCESS_READY,          /**< Created and waiting to run.             */
    PROCESS_RUNNING,        /**< Currently executing.                    */
    PROCESS_SLEEPING,       /**< Suspended, waiting for an event.        */
    PROCESS_TERMINATED      /**< Finished execution; slot can be reused. */
} ProcessState;


/* =========================================================================
 * Process Control Block (PCB)
 * ========================================================================= */

typedef struct {
    /* Identity */
    uint32_t        pid;                            /**< Unique process ID.           */
    char            name[PROCESS_NAME_MAX];         /**< Human-readable process name. */
    char            path[PROCESS_PATH_MAX];         /**< Path to the executable.      */

    /* Runtime state */
    ProcessState    state;                          /**< Current process state.       */
    int             exit_code;                      /**< Exit code after termination. */

    /* Host process handle (platform-specific).
     * On Windows this stores the PROCESS_INFORMATION handles.
     * On other platforms it is unused for now. */
    void*           host_handle;                    /**< Opaque host process handle.  */
    uint32_t        host_pid;                       /**< Host-level process ID.       */

    /* -------------------------------------------------------------------
     * Reserved fields for the future TinyVM scheduler / CPU context.
     * Do NOT use these yet — they exist so the struct layout is stable.
     * ------------------------------------------------------------------- */
    uint64_t        registers[PROCESS_REGISTER_COUNT]; /**< Virtual register file. */
    uint64_t        program_counter;                   /**< Virtual PC.            */
    uint64_t        stack_pointer;                     /**< Virtual SP.            */
    void*           memory_base;                       /**< Process memory region. */
    size_t          memory_size;                       /**< Size of memory region. */

} ProcessControlBlock;


/* =========================================================================
 * Public API
 * ========================================================================= */

/**
 * Initializes the process manager.
 * Must be called once before any other process_* function.
 */
void process_init(void);

/**
 * Creates a new process from the given executable path.
 *
 * Validates the file, launches it via the host OS (Windows only for now),
 * and inserts a new entry into the process table.
 *
 * @param executable  Path to the executable file.
 * @return The PID of the new process, or PID_INVALID on failure.
 */
uint32_t process_create(const char* executable);

/**
 * Terminates a process by PID.
 *
 * On Windows this calls TerminateProcess on the host handle.
 * The slot is marked PROCESS_TERMINATED and can be reused.
 *
 * @param pid  The process ID to kill.
 * @return true if the process was found and terminated, false otherwise.
 */
bool process_kill(uint32_t pid);

/**
 * Checks whether a process with the given PID exists and is not terminated.
 *
 * @param pid  The process ID to check.
 * @return true if the process exists and is active.
 */
bool process_exists(uint32_t pid);

/**
 * Returns a pointer to the PCB for the given PID.
 *
 * @param pid  The process ID to look up.
 * @return Pointer to the PCB, or NULL if not found.
 */
ProcessControlBlock* process_get(uint32_t pid);

/**
 * Updates the process table.
 *
 * Polls each running process to check if it has exited on the host side.
 * If so, the entry is moved to PROCESS_TERMINATED and the exit code is stored.
 * Should be called periodically (e.g. once per frame / tick).
 */
void process_update(void);

/**
 * Shuts down the process manager.
 *
 * Kills all still-running processes and resets the process table.
 */
void process_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* PROCESS_H */
