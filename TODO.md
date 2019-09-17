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
  - [x] util/
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

## Current

- [ ] Set project names as variable in CMakeLists.txt files
- [ ] Set up GitHub pages for the repository
- [ ] Add Doxygen file headers
- [ ] Set up Doxygen documentation at https://mitogen.github.io/genie/doc/
- [ ] Set up code coverage system at https://mitogen.github.io/genie/codecov/ and https://codecov.io/gh/mitogen/genie/
- [ ] Set up Google Test
- [ ] Check CLI options (should be something like: --input-file)
- [ ] Don't let genie produce genie.log by default
- [ ] Make genie work for void test file
- [ ] Check file and folder names
- [ ] Check conformance to development guidelines
- [ ] Check use and linking of OpenMP
- [ ] Run everything with util/autopep8-pycodestyle.sh and add util/autopep8-pycodestyle.sh to util/before-commit.sh

## For later

- [ ] Check LICENSE
- [ ] Unify I/O
  - [ ] Bitstream R/W should be done using HM's bitstream classes adapted for 64 bits
  - [ ] "Ordinary" file R/W (for e.g. SPRING's temporary files) should be done using the C++ standard library
- Social media image (on GitHub)
- Virtual dtor in all classes that something is being derived from (also applies to CALQ's File- class hierachy)
- Check correct use of smart pointers
- Valgrind checks
- Python interfaces
  - Only use numpy arrays
  - Just as the example "c-and-python"
