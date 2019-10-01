# Development guidelines

## C/C++

### File names

C and C++ header files have the extension ``.h``.

C source files have the extension ``.c``.

C++ source files have the extension ``.cc``.

File names are *not* written in CamelCase and do *not* contain underscores (i.e., ``_``). Use dashes (``-``) instead, as e.g. in ``my-new-module.cc``.

### Includes

Including of a library header should look like: ``#include <gabac/gabac.h>``.

Including of internal headers should look like: ``#include "internal.h"``.

### Macros

Macros have a prefix, identifying the library to which they belong, e.g. ``CALQ_``, ``GABAC_``, ``GENIE_``, ``SPRING_`` etc.

### Namespaces

Applications and libraries each have their own namespace, e.g. ``calq::``, ``gabac::``, ``genie::``, ``spring::`` etc.

### Coding style

C++ code is formatted with [clang-format](https://clang.llvm.org/docs/ClangFormat.html). See also the ``.clang-format`` file.

## Python

Python code is formatted with [autopep8](https://pypi.org/project/autopep8/0.8/).
