#ifndef CRAFT_H
#define CRAFT_H

#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _WINUSER_
#define _WINGDI_
#define _IMM_
#define _WINCON_
#include <windows.h>
#include <direct.h>
#include <shellapi.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#define CRAFT_UNREACHABLE(message)                                                \
    do                                                                            \
    {                                                                             \
        fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); \
        abort();                                                                  \
    } while (0)

#define CRAFT_COLOR_RED "\x1b[31m"
#define CRAFT_COLOR_GREEN "\x1b[32m"
#define CRAFT_COLOR_YELLOW "\x1b[33m"
#define CRAFT_COLOR_BLUE "\x1b[34m"
#define CRAFT_COLOR_MAGENTA "\x1b[35m"
#define CRAFT_COLOR_CYAN "\x1b[36m"
#define CRAFT_COLOR_RESET "\x1b[0m"

typedef enum
{
    CRAFT_INFO,
    CRAFT_SUCCESS,
    CRAFT_WARNING,
    CRAFT_ERROR,
} CraftLogLevel;

void craft_log(CraftLogLevel log_level, const char *msg, ...);

bool craft_mkdir_if_doesnt_exist(const char *path);                    // Creates a directory if it doesn't exist.
bool craft_run_cmd(const char *cmd);                                   // NOTE: Will be changed in the future.
bool craft_copy_file(const char *src_path, const char *dst_path);      // Copies file.
bool craft_delete_file(const char *file_path);                         // Deletes file.
bool craft_copy_directory(const char *src_path, const char *dst_path); // Copies entire directory.
bool craft_delete_directory(const char *path);

#ifdef CRAFT_IMPLEMENTATION

void craft_log(CraftLogLevel log_level, const char *msg, ...)
{
    switch (log_level)
    {
    case CRAFT_INFO:
        fprintf(stderr, "[INFO] ");
        break;
    case CRAFT_SUCCESS:
        fprintf(stderr, CRAFT_COLOR_GREEN "[INFO] ");
        break;
    case CRAFT_WARNING:
        fprintf(stderr, CRAFT_COLOR_YELLOW "[WARNING] ");
        break;
    case CRAFT_ERROR:
        fprintf(stderr, CRAFT_COLOR_RED "[ERROR] ");
        break;
    default:
        CRAFT_UNREACHABLE("craft_log");
    }

    fprintf(stderr, CRAFT_COLOR_RESET);

    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
}

bool craft_mkdir_if_doesnt_exist(const char *path)
{
#ifdef _WIN32
    int result = _mkdir(path);
#else
    int result = mkdir(path, 0755);
#endif
    if (result < 0)
    {
        if (errno == EEXIST)
        {
            craft_log(CRAFT_INFO, "Directory `%s` already exists", path);
            return true;
        }
        craft_log(CRAFT_ERROR, "Could not create directory `%s`: %s", path, strerror(errno));
        return false;
    }

    printf("Created directory `%s`\n", path);
    return true;
}

bool craft_run_cmd(const char *cmd)
{
    int result = system(cmd);
    if (result == 0)
    {
        craft_log(CRAFT_INFO, "Command `%s` executed successfully", cmd);
        return true;
    }
    else
    {
        craft_log(CRAFT_ERROR, "Command `%s` failed to execute", cmd);
        return false;
    }
}

bool craft_delete_file(const char *file_path)
{
    craft_log(CRAFT_INFO, "deleting %s", file_path);

    if (remove(file_path) < 0)
    {
        craft_log(CRAFT_ERROR, "Failed to delete file %s: %s", file_path, strerror(errno));
        return false;
    }
    return true;
}

bool craft_copy_directory(const char *src_path, const char *dst_path)
{
    // TODO: Use custom solution.
    craft_log(CRAFT_INFO, "Copying %s to %s", src_path, dst_path);

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "cp -r %s %s", src_path, dst_path);

    if (system(cmd) < 0)
    {
        craft_log(CRAFT_ERROR, "Failed to copy directory %s to %s: %s", src_path, dst_path, strerror(errno));
        return false;
    }
    return true;
}

bool craft_delete_directory(const char *path)
{
    // TODO: Use custom solution.
    craft_log(CRAFT_INFO, "Deleting %s", path);

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "rm -r %s", path);

    if (system(cmd) < 0)
    {
        craft_log(CRAFT_ERROR, "Failed to delete directory %s: %s", path, strerror(errno));
        return false;
    }
    return true;
}

#endif // CRAFT_IMPLEMENTATION

#endif // CRAFT_H