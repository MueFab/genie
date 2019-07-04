# Development guidelines

# File names

C and C++ header files have the extension ``.h``.

C source files have the extension ``.c``.

C++ source files have the extension ``.cc``.

File names are *not* written in CamelCase and do *not* contain underscores (i.e., ``_``). Use dashes (``-``) instead, as e.g. in ``my-new-module.cc``.

# Includes

Including of a library header should look like: ``#include <gabac/gabac.h>``.

Including of an application or library internal header should look like: ``#include "internal.h"``.

# Namespaces

Applications and libraries each have their own namespace, e.g. ``calq::``, ``gabac::``, ``genie::``, ``spring::`` etc. No further namespace hierarchies are allowed.

# Coding style

C++ code is checked with cpplint (https://github.com/cpplint/cpplint) and clang-format (https://clang.llvm.org/docs/ClangFormat.html). See also ``source/CPPLINT.cfg``.

Python code is checked with pycodestyle (https://github.com/PyCQA/pycodestyle).
