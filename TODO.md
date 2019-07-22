# TODO

## Urgent

- [x] Rename files
  - [x] C and C++ header files should have the extension ``.h``
  - [x] C source files should have the extension ``.c``.
  - [x] C++ source files should have the extension ``.cc``
  - [x] File names are *not* written in CamelCase and do *not* contain underscores (i.e., ``_``) Dashes (``-``) should be used instead, as e.g. in ``my-new-module.cc``
- [ ] Rename directories
- [x] Clean up ``#include``s
  - [x] Including of a library header should look like: ``#include <gabac/gabac.h>``
  - [x] Including of an application or library internal header should look like: ``#include "internal.h"``
- [ ] Fix namespaces
  - [ ] Applications and libraries each should have their own namespace, e.g. ``calq::``, ``gabac::``, ``genie::``, ``spring::`` etc.
- [x] Make a ``utils``library containing constants, exceptions, log functionality, etc.
- [x] Fix compiler warnings
- [ ] Add code coverage report system

## For later

- [ ] Check coding style with cpplint, clang-format and pycodestyle
- [x] Clean up ``source/libs/format/CMakeLists.txt``
- [x] Set up CMake for ``tests``
- [ ] Add Autotools build system
