#define CRAFT_IMPLEMENTATION
#include "craft.h"

int main(void)
{
    craft_mkdir_if_doesnt_exist("build");
    craft_run_cmd("cc ./tests/hello.c -o ./build/hello");
    craft_run_cmd("./build/hello");
    return 0;
}