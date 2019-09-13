# TODO

## Folder and file status

- [ ] ./
  - [x] .git/
  - [ ] .github/
    - [ ] ISSUE_TEMPLATE
      - [ ] bug_report.md
      - [ ] custom.md
      - [ ] feature_request.md
  - [ ] ci/
    - [x] cmake-build-release.sh
    - [ ] test-gabac-roundtrips.sh
    - [ ] test-genie-roundtrip.sh
  - [ ] cmake/
  - [ ] doc/
  - [ ] resources/
  - [ ] scripts/
  - [ ] source/
  - [ ] tests/

## New

- [ ] Set up Doxygen documentation
- [ ] Set up Google Test
- [ ] Push doc to gh-pages
- [ ] Compare with getspc and calq repos

## Urgent

- [x] Rename files
  - [x] C and C++ header files should have the extension ``.h``
  - [x] C source files should have the extension ``.c``.
  - [x] C++ source files should have the extension ``.cc``
  - [x] File names are *not* written in CamelCase and do *not* contain underscores (i.e., ``_``) Dashes (``-``) should be used instead, as e.g. in ``my-new-module.cc``
- [x] Rename directories
- [x] Clean up ``#include``s
  - [x] Including of a library header should look like: ``#include <gabac/gabac.h>``
  - [x] Including of an application or library internal header should look like: ``#include "internal.h"``
- [ ] Fix namespaces
  - [ ] Applications and libraries each should have their own namespace, e.g. ``calq::``, ``gabac::``, ``genie::``, ``spring::`` etc.
- [x] Make a ``utils``library containing constants, exceptions, log functionality, etc.
- [x] Fix compiler warnings
- [ ] Add code coverage report system (copy from CALQ)
- [ ] apply-clang-format.sh (copy from CALQ)
- [ ] Doxygen

## For later

- [ ] Check coding style with clang-format and pycodestyle
- [x] Clean up ``source/libs/format/CMakeLists.txt``
- [x] Set up CMake for ``tests``
- [ ] Unify I/O
  - [ ] Bitstream R/W should be done using HM's bitstream classes adapted for 64 bits
  - [ ] "Ordinary" file R/W (for e.g. SPRING's temporary files) should be done using the C++ standard library

## Development modules

- Version control
- Travis CI
- GitHub
- Doxygen
- GitHub pages
- codecov.io
