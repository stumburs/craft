#define CRAFT_IMPLEMENTATION
#include "../craft.h"

int main(void)
{
    craft_log(CRAFT_INFO, "This is an info message.");
    craft_log(CRAFT_SUCCESS, "This is a success message.");
    craft_log(CRAFT_WARNING, "This is a warning message.");
    craft_log(CRAFT_ERROR, "This is an error message.");

    const char *str = "Cool!";
    craft_log(CRAFT_INFO, "This is a message using string formatting. %s", str);
    return 0;
}