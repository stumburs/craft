#define CRAFT_IMPLEMENTATION
#include "craft.h"

// Create a dynamic array of a custom type
craft_array_define(IntArray, int);

int main(void)
{
    // Create command
    CraftCMD cmd = {0};

    // Construct command
    craft_cmd_append(&cmd, "cc", "craft.c", "-o", "craft_test", "-ggdb");

    // Run and 'clear' command for next use
    craft_cmd_run_reset(&cmd);

    // Initialize custom array
    IntArray arr = {0};

    // Use IntArray
    for (size_t i = 0; i < 10; i++)
    {
        craft_array_push(&arr, rand() % 10);
        printf("%d\t", arr.items[i]);
    }
    printf("\n");

    // Re-use the same cmd variable for a new command
    craft_cmd_append(&cmd, "rm", "craft_test");
    craft_cmd_run_reset(&cmd);

    // Free array
    craft_array_free(arr);
    // Free cmd
    craft_cmd_free(cmd);
    return 0;
}
