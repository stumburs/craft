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
    CRAFT_WARNING,
    CRAFT_ERROR,
} CraftLogLevel;

void craft_log(CraftLogLevel log_level, const char *msg, ...);

bool craft_mkdir_if_doesnt_exist(const char *path); // Creates a directory if it doesn't exist.
bool craft_run_cmd(const char *cmd);                // NOTE: Will be changed in the future.
bool craft_copy_file(const char *src_path, const char *dst_path);

#ifdef CRAFT_IMPLEMENTATION

void craft_log(CraftLogLevel log_level, const char *msg, ...)
{
    switch (log_level)
    {
    case CRAFT_INFO:
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
            printf("Directory `%s` already exists\n", path);
            return true;
        }
        printf("Could not create directory `%s`: %s\n", path, strerror(errno));
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
        printf("Command `%s` executed successfully\n", cmd);
        return true;
    }
    else
    {
        printf("Command execution failed\n");
        return false;
    }
}

#endif // CRAFT_IMPLEMENTATION

#endif // CRAFT_H