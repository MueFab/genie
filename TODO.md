# TODO

## Folder and file status

- [ ] ./
  - [x] .github/
    - [x] ISSUE_TEMPLATE
      - [x] bug_report.md
      - [x] custom.md
      - [x] feature_request.md
  - [x] ci/
    - [x] cmake-build-debug-all.sh
    - [x] cmake-build-release.sh
    - [x] report-coverage.sh
    - [x] run-gabac-roundtrips.sh
    - [x] run-genie-roundtrip.sh
    - [x] run-tests.sh
    - [ ] test-gabac-python-api.sh
  - [ ] cmake/
    - [ ] Doxygen.cmake
    - [ ] GoogleTest.cmake
    - [ ] CMakeListsGoogleTest.txt.in
  - [ ] doc/
  - [ ] resources/
  - [ ] source/
  - [ ] tests/
  - [ ] util/
    - [x] authors.sh
    - [x] autopep8-pycodestyle.sh
    - [x] before-commit.sh
    - [x] clang-format.sh
    - [x] cmp-reordered-fastq.sh
    - [x] conformance-roundtrip.sh
    - [x] shellcheck.sh
  - [x] .clang-format
  - [x] .gitignore
  - [ ] .travis.yml
  - [x] AUTHORS
  - [x] CMakeLists.txt
  - [x] CODE_OF_CONDUCT.md
  - [x] CONTRIBUTING.md
  - [x] Doxyfile.in
  - [x] LICENSE
  - [x] PULL_REQUEST_TEMPLATE.md
  - [ ] README.md
  - [ ] TODO.md
  - [x] USAGE_POLICY.md

## New

- [ ] Set up Doxygen documentation
- [ ] Set up Google Test
- [ ] Push doc to gh-pages
- [ ] Compare with getspc and calq repos
- [ ] Check CLI options (should be something like: --input-file)
- [ ] Don't let genie produce genie.log by default
- [ ] Make genie work for void test file
- [ ] Rename scripts/ to util/

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
- Social media image (on GitHub)
- Virtual dtor in all classes that something is being derived from (also applies to CALQ's File- class hierachy)
- Remove Autotools stuff
- Check correct use of smart pointers
- Valgrind checks
- Apps
  - genie
  - ureads-encoder
  - local-assembly-encoder
- Python interfaces
  - Only use numpy arrays
  - Just as the example "c-and-python"
- Clean up branches & branch names

## Development modules

- Version control
- Travis CI
- GitHub pages
  - Doxygen documentation: https://mitogen.github.io/genie/doc/
  - Codecov report:
    - https://mitogen.github.io/genie/codecov/
    - codecov.io (both locations)
- Doxygen
- GitHub pages
- codecov.io
