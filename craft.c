#define CRAFT_IMPLEMENTATION
#include "craft.h"

int main(void)
{
    CraftCMD cmd = {0};
    craft_cmd_append(&cmd, "cc", "craft.c", "-o", "craft_test", "-ggdb");
    craft_cmd_run_reset(&cmd);

    craft_cmd_append(&cmd, "rm", "craft_test");
    craft_cmd_run_reset(&cmd);
    return 0;
}