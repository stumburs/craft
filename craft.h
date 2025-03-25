#ifndef CRAFT_H
#define CRAFT_H

#include <stddef.h>
#include <stdarg.h>
#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

// ====================
// Dynamic arrays
// ====================

// Initial dynamic array capacity
#define CRAFT_ARRAY_INIT_CAP 4

// Appends item to dynamic array
#define craft_array_push(arr, value)                                                             \
    do                                                                                           \
    {                                                                                            \
        if ((arr)->count >= (arr)->capacity)                                                     \
        {                                                                                        \
            (arr)->capacity = (arr)->capacity == 0 ? CRAFT_ARRAY_INIT_CAP : (arr)->capacity * 2; \
            (arr)->items = realloc((arr)->items, (arr)->capacity * sizeof(*(arr)->items));       \
            assert((arr)->items != NULL && "Buy more RAM xdd");                                  \
        }                                                                                        \
        (arr)->items[(arr)->count++] = (value);                                                  \
    } while (0)

// Appends multiple items to dynamic array
#define craft_array_push_many(arr, new_items, new_items_count)                                       \
    do                                                                                               \
    {                                                                                                \
        if ((arr)->count + (new_items_count) > (arr)->capacity)                                      \
        {                                                                                            \
            if ((arr)->capacity == 0)                                                                \
            {                                                                                        \
                (arr)->capacity = CRAFT_ARRAY_INIT_CAP;                                              \
            }                                                                                        \
            while ((arr)->count + (new_items_count) > (arr)->capacity)                               \
            {                                                                                        \
                (arr)->capacity *= 2;                                                                \
            }                                                                                        \
            (arr)->items = realloc((arr)->items, (arr)->capacity * sizeof(*(arr)->items));           \
            assert((arr)->items != NULL && "Buy more RAM xdd");                                      \
        }                                                                                            \
        memcpy((arr)->items + (arr)->count, (new_items), (new_items_count) * sizeof(*(arr)->items)); \
        (arr)->count += (new_items_count);                                                           \
    } while (0)

// Frees memory allocated in dynamic array
#define craft_array_free(arr) free((arr).items)

// Might cause some IDEs to complain about missing definition.
#define craft_array_define(name, type) \
    typedef struct name##_t            \
    {                                  \
        type *items;                   \
        size_t count;                  \
        size_t capacity;               \
    } name;

// ====================
// Pre-defined dynamic arrays
// ====================

// char* dynamic array
typedef struct
{
    char **items;
    size_t count;
    size_t capacity;
} CraftStringDA;

// ====================
// String builder
// ====================

// String builder struct
typedef struct
{
    char *items;
    size_t count;
    size_t capacity;
} CraftStringBuilder;

// Appends buffer to stringbuilder
#define craft_sb_append_buf(sb, buf, size) craft_array_push_many(sb, buf, size)

// Appends a c-string to stringbuilder
#define craft_sb_append_cstr(sb, cstr)   \
    do                                   \
    {                                    \
        const char *s = (cstr);          \
        size_t n = strlen(s);            \
        craft_array_push_many(sb, s, n); \
    } while (0)

// Null-terminates stringbuilder
#define craft_sb_append_null(sb) craft_array_push_many(sb, "", 1)

// Frees memory allocated in stringbuilder
#define craft_sb_free(sb) free((sb).items)

// ====================
// CMD
// ====================

// Command struct
typedef CraftStringDA CraftCMD;

// Appends multiple arguments to command
#define craft_cmd_append(cmd, ...)                         \
    craft_array_push_many(cmd,                             \
                          ((const char *[]){__VA_ARGS__}), \
                          (sizeof((const char *[]){__VA_ARGS__}) / sizeof(const char *)))

// Renders command into CraftStringBuilder string
void craft_cmd_render(CraftCMD cmd, CraftStringBuilder *render);
// Executes command
bool craft_cmd_run(CraftCMD cmd);
// Executes command and clears it
bool craft_cmd_run_reset(CraftCMD *cmd);
// Frees memory allocated in command
#define craft_cmd_free(cmd) free(cmd.items)

// ====================
// Logging
// ====================

// Log types
typedef enum
{
    CRAFT_INFO,
    CRAFT_WARNING,
    CRAFT_ERROR,
} CraftLogType;

// Text colors
#define CRAFT_BLACK "\e[0;30m"
#define CRAFT_RED "\e[0;31m"
#define CRAFT_GREEN "\e[0;32m"
#define CRAFT_YELLOW "\e[0;33m"
#define CRAFT_BLUE "\e[0;34m"
#define CRAFT_MAGENTA "\e[0;35m"
#define CRAFT_CYAN "\e[0;36m"
#define CRAFT_WHITE "\e[0;37m"
#define CRAFT_RESET "\e[0m"

// printf-like logging
void craft_log(CraftLogType type, const char *fmt, ...);

// ====================
// Filesystem
// ====================

// Checks if path is a valid directory
bool craft_is_dir(const char *path);
// Creates directory
bool craft_create_dir(const char *path);
// Removes directory (if empty)
bool craft_remove_dir(const char *path);
// Removes directory, including all subdirectories AND files
bool craft_remove_dir_recursive(const char *path);
// Appends `files` with file names in directory (not including subdirectories)
bool craft_get_files_in_dir(const char *path, CraftStringDA *files);
// Removes file
bool craft_remove_file(const char *path);
// Removes files.
bool craft_remove_files(CraftStringDA *files);
// Moves file to target path
bool craft_move_file(const char *src, const char *dest);
// Moves files to target path
bool craft_move_files(CraftStringDA *files, const char *dest);
// Copies file to target path
bool craft_copy_file(const char *src, const char *dest);
// Copies files to target path
bool craft_copy_files(CraftStringDA *files, const char *dest);

#ifdef CRAFT_IMPLEMENTATION

inline void craft_cmd_render(CraftCMD cmd, CraftStringBuilder *render)
{
    for (size_t i = 0; i < cmd.count; i++)
    {
        const char *arg = cmd.items[i];
        if (arg == NULL)
            break;
        if (i > 0)
            craft_sb_append_cstr(render, " ");
        if (!strchr(arg, ' '))
        {
            craft_sb_append_cstr(render, arg);
        }
        else
        {
            craft_array_push(render, '\'');
            craft_sb_append_cstr(render, arg);
            craft_array_push(render, '\'');
        }
    }
}

inline bool craft_cmd_run(CraftCMD cmd)
{
    bool result = true;
    if (cmd.count < 1)
    {
        craft_log(CRAFT_WARNING, "Empty command");
        return false;
    }
    CraftStringBuilder sb = {0};
    craft_cmd_render(cmd, &sb);
    craft_sb_append_null(&sb);
#ifndef CRAFT_SILENT
    craft_log(CRAFT_INFO, "CMD: %s", sb.items);
#endif
    if (system(sb.items) != 0)
    {
        result = false;
    };
    craft_sb_free(sb);
    memset(&sb, 0, sizeof(sb));
    return result;
}

inline bool craft_cmd_run_reset(CraftCMD *cmd)
{
    bool result = craft_cmd_run(*cmd);
    cmd->count = 0;
    return result;
}

// ====================
// Logging
// ====================
inline void craft_log(CraftLogType type, const char *fmt, ...)
{
#ifdef CRAFT_SILENT
    return;
#endif
    switch (type)
    {
    case CRAFT_INFO:
        fprintf(stderr, "[INFO] ");
        break;
    case CRAFT_WARNING:
        fprintf(stderr, CRAFT_YELLOW "[WARNING] " CRAFT_RESET);
        break;
    case CRAFT_ERROR:
        fprintf(stderr, CRAFT_RED "[ERROR] " CRAFT_RESET);
        break;
    default:
        break;
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

// ====================
// Filesystem
// ====================
inline bool craft_is_dir(const char *path)
{
    struct stat path_stat;
    if (stat(path, &path_stat) != 0)
    {
        return false;
    }

    return S_ISDIR(path_stat.st_mode);
}

inline bool craft_create_dir(const char *path)
{
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
    {
        craft_log(CRAFT_INFO, "Directory `%s` already exists", path);
        return true;
    }

    if (mkdir(path, 0777) == 0)
    {
        craft_log(CRAFT_INFO, "Created directory `%s`", path);
        return true;
    }

    craft_log(CRAFT_ERROR, "Failed to create directory `%s`: %s", path, strerror(errno));
    return false;
}

inline bool craft_remove_dir(const char *path)
{
    struct stat st;

    if (stat(path, &st) != 0)
    {
        if (errno == ENOENT)
        {
            craft_log(CRAFT_WARNING, "Directory `%s` doesn't exist", path);
            return true;
        }
        craft_log(CRAFT_ERROR, "stat failed: %s", strerror(errno));
        return false;
    }

    if (!S_ISDIR(st.st_mode))
    {
        craft_log(CRAFT_ERROR, "%s is not a directory", path);
        return false;
    }

    if (rmdir(path) == 0)
    {
        craft_log(CRAFT_INFO, "Removed directory `%s`", path);
        return true;
    }

    craft_log(CRAFT_ERROR, "Failed to remove directory `%s`: %s", path, strerror(errno));
    return false;
}

inline bool craft_remove_dir_recursive(const char *path)
{
    struct stat st;

    if (stat(path, &st) != 0)
    {
        if (errno == ENOENT)
        {
            craft_log(CRAFT_WARNING, "Directory `%s` doesn't exist", path);
            return true;
        }
        craft_log(CRAFT_ERROR, "stat failed: %s", strerror(errno));
        return false;
    }

    if (!S_ISDIR(st.st_mode))
    {
        craft_log(CRAFT_ERROR, "%s is not a directory", path);
        return false;
    }

    DIR *dir = opendir(path);
    if (!dir)
    {
        craft_log(CRAFT_ERROR, "Failed to open directory `%s`: %s", path, strerror(errno));
        return false;
    }

    struct dirent *entry;
    char full_path[1024]; // TODO: Use custom temp strings

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip `.` and `..`
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name); // TODO: Use custom temp strings

        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode))
        {
            if (!craft_remove_dir_recursive(full_path))
            {
                closedir(dir);
                craft_log(CRAFT_ERROR, "Failed to remove directory `%s` within `%s`: %s", entry->d_name, path, strerror(errno));
                return false;
            }
        }

        else
        {
            if (unlink(full_path) != 0)
            {
                closedir(dir);
                craft_log(CRAFT_ERROR, "Failed to unlink directory `%s` within `%s`: %s", entry->d_name, path, strerror(errno));
                return false;
            }
        }
    }

    closedir(dir);

    if (rmdir(path) != 0)
    {
        craft_log(CRAFT_ERROR, "Failed to remove directory `%s`: %s", path, strerror(errno));
        return false;
    }

    craft_log(CRAFT_INFO, "Removed directory (recursively) `%s`", path);
    return true;
}

inline bool craft_get_files_in_dir(const char *path, CraftStringDA *files)
{
    struct stat st;

    if (stat(path, &st) != 0 || !S_ISDIR(st.st_mode))
    {
        craft_log(CRAFT_ERROR, "`%s` is not a directory or doesn't exist: %s", path, strerror(errno));
        return false;
    }

    DIR *dir = opendir(path);
    if (!dir)
    {
        craft_log(CRAFT_ERROR, "Failed to open `%s`: `%s`", path, strerror(errno));
        return false;
    }

    struct dirent *entry;
    char full_path[1024]; // TODO: replace with custom temp sprintf

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip '.' and '..'
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode))
        {
            continue;
        }

        craft_array_push(files, strndup(entry->d_name, strlen(entry->d_name))); // Not sure if strndup is the best solution. Don't really wanna use plain strdup
    }

    closedir(dir);
    return true;
}

inline bool craft_remove_file(const char *path)
{
    if (!path)
    {
        craft_log(CRAFT_ERROR, "`path` is NULL");
        return false;
    }

    if (unlink(path) != 0)
    {
        craft_log(CRAFT_ERROR, "Failed to remove `%s`: ", path, strerror(errno));
        return false;
    }
    else
    {
        craft_log(CRAFT_INFO, "Removed `%s`", path);
        return true;
    }
}

inline bool craft_remove_files(CraftStringDA *files)
{
    if (!files)
    {
        craft_log(CRAFT_ERROR, "`files` are NULL");
        return false;
    }
    if (files->count == 0)
    {
        craft_log(CRAFT_ERROR, "`files` are empty");
        return false;
    }

    bool success = true;

    for (size_t i = 0; i < files->count; i++)
    {
        if (!craft_remove_file(files->items[i]))
        {
            success = false;
        }
    }
    return success;
}

inline bool craft_move_file(const char *src, const char *dest)
{
    if (!src)
    {
        craft_log(CRAFT_ERROR, "`src` is NULL");
        return false;
    }
    if (!dest)
    {
        craft_log(CRAFT_ERROR, "`dest` is NULL");
        return false;
    }

    char dest_path[1024];
    snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, strrchr(src, '/') ? strrchr(src, '/') + 1 : src);

    if (rename(src, dest_path) != 0)
    {
        craft_log(CRAFT_ERROR, "Failed to move file `%s` to `%s`: %s", src, dest, strerror(errno));
        return false;
    }
    else
    {
        craft_log(CRAFT_INFO, "Moved `%s` to `%s`", src, dest);
        return true;
    }
}

inline bool craft_move_files(CraftStringDA *files, const char *dest)
{
    if (!files)
    {
        craft_log(CRAFT_ERROR, "`files` are NULL");
        return false;
    }
    if (files->count == 0)
    {
        craft_log(CRAFT_ERROR, "`files` are empty");
        return false;
    }
    if (!dest)
    {
        craft_log(CRAFT_ERROR, "`dest` is NULL");
        return false;
    }

    if (!craft_is_dir(dest))
    {
        craft_log(CRAFT_ERROR, "`%s` is not a valid directory: %s", dest, strerror(errno));
        return false;
    }

    bool success = true;
    for (size_t i = 0; i < files->count; i++)
    {
        if (!craft_move_file(files->items[i], dest))
        {
            success = false;
        }
    }

    return success;
}

inline bool craft_copy_file(const char *src, const char *dest)
{
    FILE *src_file = fopen(src, "rb");
    if (!src_file)
    {
        craft_log(CRAFT_ERROR, "Failed to open source file `%s`: ", src, strerror(errno));
        return false;
    }

    FILE *dest_file = fopen(dest, "wb");
    if (!dest_file)
    {
        craft_log(CRAFT_ERROR, "Failed to open destination file `%s`: ", dest, strerror(errno));
        fclose(src_file);
        return false;
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes == fread(buffer, 1, sizeof(buffer), src_file)) > 0)
    {
        fwrite(buffer, 1, bytes, dest_file);
    }

    fclose(src_file);
    fclose(dest_file);

    craft_log(CRAFT_INFO, "Copied `%s` to `%s", src, dest);

    return true;
}

inline bool craft_copy_files(CraftStringDA *files, const char *dest)
{
    if (!files)
    {
        craft_log(CRAFT_ERROR, "`files` are NULL");
        return false;
    }
    if (files->count == 0)
    {
        craft_log(CRAFT_ERROR, "`files` are empty");
        return false;
    }
    if (!dest)
    {
        craft_log(CRAFT_ERROR, "`dest` is NULL");
        return false;
    }

    if (!craft_is_dir(dest))
    {
        craft_log(CRAFT_ERROR, "`%s` is not a valid directory: %s", dest, strerror(errno));
        return false;
    }

    bool success = true;
    for (size_t i = 0; i < files->count; i++)
    {
        if (!craft_copy_file(files->items[i], dest))
        {
            success = false;
        }
    }

    return success;
}

#endif // CRAFT_IMPLEMENTATION
#endif // CRAFT_H