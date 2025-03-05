#ifndef CRAFT_H
#define CRAFT_H

#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

bool craft_mkdir_if_doesnt_exist(const char *path); // Creates a directory if it doesn't exist.
bool craft_run_cmd(const char *cmd);                // NOTE: Will be changed in the future.

#ifdef CRAFT_IMPLEMENTATION

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