#define CRAFT_IMPLEMENTATION
#include "craft.h"

int main(void)
{
    craft_mkdir_if_doesnt_exist("build");
    return 0;
}