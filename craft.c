#define CRAFT_IMPLEMENTATION
#include "craft.h"
#include <assert.h>

int main(void)
{
    craft_log(CRAFT_INFO, "This is an info message.");
    craft_log(CRAFT_WARNING, "This is a warning message.");
    craft_log(CRAFT_ERROR, "This is an error message.");

    assert(craft_mkdir_if_doesnt_exist("build") == true);
    assert(craft_run_cmd("cc ./tests/hello.c -o ./build/hello") == true);
    assert(craft_run_cmd("./build/hello") == true);
    return 0;
}