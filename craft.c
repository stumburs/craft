#define CRAFT_IMPLEMENTATION
#include "craft.h"
#include <assert.h>

#define BUILD_PATH "build/"
#define TESTS_PATH "tests/"

const char *test_names[] = {
    "hello",
    "log",
    "directories",
};

bool build_and_run_test(const char *cmd, const char *test_name)
{
    craft_log(CRAFT_INFO, "----- Running test: %s -----", test_name);

    // TODO: Introduce custon command struct
    char bin_path[1024];
    char src_path[1024];
    char full_cmd[4096];

    snprintf(bin_path, sizeof(bin_path), "%s%s", BUILD_PATH TESTS_PATH, test_name);
    snprintf(src_path, sizeof(src_path), "%s%s.c", TESTS_PATH, test_name);
    snprintf(full_cmd, sizeof(full_cmd), "%s %s -o %s", cmd, src_path, bin_path);

    // Build executable
    if (!craft_run_cmd(full_cmd))
    {
        return false;
    }

    snprintf(full_cmd, sizeof(full_cmd), "./%s%s", BUILD_PATH TESTS_PATH, test_name);

    // Run executable
    if (!craft_run_cmd(full_cmd))
    {
        return false;
    }

    craft_log(CRAFT_SUCCESS, "----- Test succesful: %s -----\n", bin_path);

    return true;
}

int main(void)
{

    if (!craft_mkdir_if_doesnt_exist(BUILD_PATH))
        return 1;
    if (!craft_mkdir_if_doesnt_exist(BUILD_PATH TESTS_PATH))
        return 1;

    for (size_t i = 0; i < sizeof(test_names) / sizeof(const char *); i++)
    {
        if (!build_and_run_test("cc", test_names[i]))
            return 1;
    }

    craft_log(CRAFT_SUCCESS, "====================");
    craft_log(CRAFT_SUCCESS, "All tests passed!");
    craft_log(CRAFT_SUCCESS, "====================");
    return 0;
}