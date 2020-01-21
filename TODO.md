# TODO

---

- Re-use ID and QV compression code from SPRING
- Set up ref SW x-check scripts
- Re-enable CI on Travis CI
- Tools
    - Implement FASTQ/SAM-to-MPEG-G records tool
    - Implement tool to compare two MPEG-G records arrays with different orders
- [ ] Implement part 1 en-/decapsulator
- [ ] Write more unit tests
- [ ] Add support for >1 transformed sequences to format part 2 library    
- [ ] Implement QV-Codebook in Part2 library
- [ ] Implement C++ Core Guideline C.21 ("If you define or =delete any default operation, define or =delete them all")
    

- Nice to have:
    - Lossy ID compression
    - Support for descriptor specific GABAC configurations
    - Reference-based encoder
    - Implement LZMA coding of auxiliary fields
- Code cleanup:
    - Unify data structures
    - Unify I/O
- Checks:
    - Check whether adaptive entropy coding (including binarization et al.) is conformant
    - Check whether LUTs are conformant
    - Check BitInputStream::read(): all statements like readIn(&m_reader) are source of potential bug for wordSize > 1
    - Check support fot > 1 transformed subsequence in part 2 library
- Use clang-format



- [ ] Clean up collaborators on GitHub
- [ ] Remove GABAC optimization stuff
- [ ] Add alphabet lookup for sclips descriptor
- [ ] Update background image on the Genie page
- [ ] Update MITOGEN page
- [ ] Add boilerplate to all source code files
- [ ] Check whether everything builds nicely on Travis CI
- [ ] Enable all tests in .travis.yml
- [ ] Enable util-tests in ci/generate-coverage-report.sh
- [ ] Enable checks in util/autopep8-pycodestyle.sh
- [ ] Don't let genie produce genie.log by default
- [ ] Rename command line options to something like --input-file instead of --input_file or --input-file-path (this applies to *all* applications)
- [ ] Check comments
  - [ ] Sentences should start with an uppercase letter and end with a full stop.
  - [ ] Statements should start with an uppercase letter and not end with a full stop.
- [ ] Clean up exceptions (use only a simple class derived from std::exception - without all the special things)
- [ ] Implement a unified logging solution
- [ ] Format log and error messages uniformly
- [ ] Check for correct usage of assertions versus exceptions
- [ ] Make destructors virtual in all classes that something is being derived from (use override in derived classes)
- [ ] Unify I/O
  - [ ] Bitstream I/O should be done using HM's bitstream classes adapted for 64 bits
  - [ ] "Ordinary" file I/O (for e.g. SPRING's temporary files) should be done using the C++ standard library
- [ ] Remove Python interfaces
- [ ] In src/libs/gabac/binary-arithmetic-decoder.cc:112 4 bytes must be skipped. This would not be necessary if bitInputStream and binaryArithmeticDecoder would be using references to the same underlying bitstream.
- [ ] Announce use of GitHub issues


- [ ] Investigate GABAC roundtrip ("u(32) issue")
- [ ] Check whether adaptive entropy coding works in conformance tests


- [ ] Fix clang-tidy warnings
- [ ] Fix compilation warnings
- [ ] Fix Doxygen warnings
- [ ] Clean repository with: https://rtyley.github.io/bfg-repo-cleaner/
- [ ] Check conformance to doc/development-guidelines.md
- [ ] Check everything with Valgrind
- [ ] Resolve FIXMEs and TODOs
- [ ] Update README.md
- [ ] Update doc/gabac.md
- [ ] Check LICENSE (check that it is compatible to SPRING's license)
- [ ] Design social media image (for GitHub)

## Issues

- For LUTs, instead of encoding/decoding the symbols in the order of their frequency (as specified in the spec), their frequencies are encoded instead.
- Though LUTs are encoded, I am not sure if they are used as specified in the spec.
- In method BitInputStream::read(), all statements like this "readIn(&m_reader)" when they expect a byte to be returned are source of potential bug for wordSize > 1, because then the return value will not be a byte but could be of higher size corresponding to wordSize. There may be few other places to be checked.

## Futuristic things

- [ ] Replace thirdparty/filesystem/ with std::filesystem (C++17-compliant compilers are needed for that)
