# TODO

- Rename files
  - C and C++ header files should have the extension ``.h``
  - C source files should have the extension ``.c``.
  - C++ source files should have the extension ``.cc``
  - File names are *not* written in CamelCase and do *not* contain underscores (i.e., ``_``) Dashes (``-``) should be used instead, as e.g. in ``my-new-module.cc``
- Clean up ``#include``s
  - Including of a library header should look like: ``#include <gabac/gabac.h>``
  - Including of an application or library internal header should look like: ``#include "internal.h"``
- Fix namespaces
  - Applications and libraries each should have their own namespace, e.g. ``calq::``, ``gabac::``, ``genie::``, ``spring::`` etc.
- Make a ``utils``library containing constants, exceptions, log functionality, etc.
- Fix compiler warnings
- Check coding style with cpplint, clang-format and pycodestyle
- Clean up ``source/libs/format/CMakeLists.txt``
- Set up CMake for ``tests``
- Use new logger
- Add Autotools build system
- Add code coverage report system
