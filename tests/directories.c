#define CRAFT_IMPLEMENTATION
#include "../craft.h"

int main(void)
{
    if (!craft_mkdir_if_doesnt_exist("test_directories"))
        return 1;
    if (!craft_mkdir_if_doesnt_exist("test_directories/subdir"))
        return 1;
    if (!craft_copy_directory("test_directories/subdir", "test_directories/subdir2"))
        return 1;
    if (!craft_delete_directory("test_directories/subdir"))
        return 1;
    if (!craft_delete_directory("test_directories"))
        return 1;

    return 0;
}