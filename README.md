# craft

craft is a simple C header-only library used for writing build recipes in C itself. This library is heavily inspired by [tsoding/nob.h](https://github.com/tsoding/nob.h).

> [!NOTE]
> At the moment only Linux is supported.

## Features

- Whatever I needed

## Usage

To use craft, simply include the `craft.h` header file in your C project and define `CRAFT_IMPLEMENTATION` in one of your source files to include the implementation.

```c
#define CRAFT_IMPLEMENTATION
#include "craft.h"
```

### Creating Directories

You can create a directory if it doesn't exist using the `craft_mkdir_if_doesnt_exist` function.

```c
if (craft_mkdir_if_doesnt_exist("build")) {
    // Directory created or already exists
} else {
    // Failed to create directory
}
```

### Running Commands

You can run shell commands using the `craft_run_cmd` function.

```c
if (craft_run_cmd("gcc -o myprogram main.c")) {
    // Command executed successfully
} else {
    // Command execution failed
}
```

## License

This project is licensed under the MIT License.
