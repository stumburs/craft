#ifndef CRAFT_H
#define CRAFT_H

#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

// Dynamic array utilities
#define CRAFT_ARRAY_INIT_CAP 4

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

#define craft_array_free(arr) free((arr).items)

// Might cause some IDEs to complain about missing definition.
#define define_array(name, type) \
    typedef struct name##_t      \
    {                            \
        type *items;             \
        size_t count;            \
        size_t capacity;         \
    } name;

// Dynamic arrays
typedef struct
{
    char **items;
    size_t count;
    size_t capacity;
} CraftStringDA;

// String builder
typedef struct
{
    char *items;
    size_t count;
    size_t capacity;
} CraftStringBuilder;

#define craft_sb_append_buf(sb, buf, size) craft_array_push_many(sb, buf, size)

#define craft_sb_append_cstr(sb, cstr)   \
    do                                   \
    {                                    \
        const char *s = (cstr);          \
        size_t n = strlen(s);            \
        craft_array_push_many(sb, s, n); \
    } while (0)

#define craft_sb_append_null(sb) craft_array_push_many(sb, "", 1)

#define craft_sb_free(sb) free((sb).items)

// CMDs
typedef CraftStringDA CraftCMD;

#define craft_cmd_append(cmd, ...)                         \
    craft_array_push_many(cmd,                             \
                          ((const char *[]){__VA_ARGS__}), \
                          (sizeof((const char *[]){__VA_ARGS__}) / sizeof(const char *)))

void craft_cmd_render(CraftCMD cmd, CraftStringBuilder *render);
bool craft_cmd_run(CraftCMD cmd);
bool craft_cmd_run_reset(CraftCMD *cmd);
#define craft_cmd_free(cmd) free(cmd.items)

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
        fprintf(stderr, "Empty command\n");
        return false;
    }
    CraftStringBuilder sb = {0};
    craft_cmd_render(cmd, &sb);
    craft_sb_append_null(&sb);
    printf("CMD: %s\n", sb.items);
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

#endif // CRAFT_IMPLEMENTATION
#endif // CRAFT_H