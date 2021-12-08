# TODO

---

- [ ] Clean up ('legacy') tags
- [ ] Windows build?
- [ ] openMP build on macOS
- [ ] Code coverage
- [ ] Clean up build scripts (makeall.sh, removebuild.sh)
- [ ] rm TODO.md
- [ ] Update README.md & PULL_REQUEST_TEMPLATE.md
- [ ] Discuss license with OS
- [ ] Update Doxyfile.in
- [ ] Fix GitHub files
- [ ] rm AUTHORS
- [ ] rm .travis.yml
- [ ] Everybody use clang-format (git hooks?)
- [ ] Update util/
- [ ] Move to C++17
  - [ ] ghc:: -> std::
  - [ ] string_view
  - [ ] boost::optional -> std::optional
  - [ ] genie::util::make_unique -> std::make_unique
- [ ] mv test/*.sh to elsewhere
- [ ] Include contents of doc/ in DEVELOPMENT.md
- [ ] Include *.md files in Doxygen
- [ ] rm GABAC configs (data/gabac/)
- [ ] Set up conformance scripts
- [ ] rm src/misc/
- [ ] Rename src/genie/module/
- [ ] Better test files
- [ ] Clean up data/misc
- [ ] Delete web page
- [ ] Update issue templates
- [ ] Clean up ci/
- [ ] Verify exception handling
- [ ] Add GENIE_DIE macro
- [ ] Add shortcut U/I classification
- [ ] rm src/apps/encapsulator
- [ ] rm classifier-bypass
- [ ] Implement C++ Core Guideline C.21 ("If you define or =delete any default operation, define or =delete them all")
- Use clang-format
- [ ] Clean up collaborators on GitHub
- [ ] Remove GABAC optimization stuff
- [ ] Update MITOGEN page
- [ ] Add boilerplate to all source code files
- [ ] Style
  - [ ] Sentences should start with an uppercase letter and end with a full stop.
  - [ ] Statements should start with an uppercase letter and not end with a full stop.
- [ ] Clean up exceptions (use only a simple class derived from std::exception - without all the special things)
- [ ] Implement a unified logging solution
- [ ] Check for correct usage of assertions versus exceptions
- [ ] Make destructors virtual in all classes that something is being derived from (use override in derived classes)
- [ ] Unify I/O
  - [ ] Bitstream I/O should be done using HM's bitstream classes adapted for 64 bits
  - [ ] "Ordinary" file I/O (for e.g. SPRING's temporary files) should be done using the C++ standard library
- [ ] Remove Python interfaces
- [ ] In src/libs/gabac/binary-arithmetic-decoder.cc:112 4 bytes must be skipped. This would not be necessary if bitInputStream and binaryArithmeticDecoder would be using references to the same underlying bitstream.
- [ ] Announce use of GitHub issues
- [ ] Fix clang-tidy warnings
- [ ] Fix compilation warnings
- [ ] Fix Doxygen warnings
- [ ] Clean repository with: https://rtyley.github.io/bfg-repo-cleaner/
- [ ] Check conformance to doc/development-guidelines.md
- [ ] Check everything with Valgrind
- [ ] Resolve FIXMEs and TODOs
- [ ] Update README.md
- [ ] Check LICENSE (check that it is compatible to SPRING's license)

# State of fastq transcoder
- [X] Single end
- [X] Paired end
- [X] Read names
- [X] Quality values
- [ ] Missing pairs
- [ ] Adapt interface to SRS

CI tests are integrated into fastq tests due to relatively low complexity

# State of sam -> mgrec transcoder
- [X] Single end
- [X] Paired end, same record
- [X] Read names
- [X] Quality values
- [X] Flags
- [ ] Handle case of forced split, if records too far apart (16 bit delta value limitation)
- [X] Paired end, pair missing 
- [X] Paired end, other record
- [ ] Splices
- [ ] Secondary alignments
- [ ] Move classification between classes P, N, M, I from genie to transcoder
- [ ] Adapt interface to SRS
- [ ] Windows support
- [X] CI tests including SAM comparison 

The following characteristics of a sam file are lost during transcoding, as impossible to express in mpegg-records:
- Sam Flag "Supplementary alignment" 
- Distinction between CIGAR characters M, X and =
- TLEN has no strict standard, so some TLEN fields can change during transcoding
- Optional tags and the SAM file header
- Unmapped records lose the "reverse complement" flag.
- RNAME is replaced by a unique number instead of a string.

The following characteristics of a sam file are lost during transcoding, but could be, in theory, supported:
- If paired reads are mapped to different reference sequences, it is not checked if the paired record is actually present. The pointer to the paired read is just copied. Whereas if they are map to the same sequence, a missing mate will be marked as "unpaired".
- If a pair is transcoded into two different records (either because the mates are mapped to two different sequences or because they are too far apart for a single record), the "SEQ of the next segment in the template being reverse complemented" SAM flag is lost
- Splices / seondary alignments are discarded (but easy to add)
- If flags are set only for one SAM record (e.g. optical duplicate), the state of the flags in the second SAM record are lost. The case that those flags differ between SAM records of the same pair is rare. Could be solved by splitting such pairs into two MPEG records.

Windows support is currently deactivated for SAM transcoding, as it uses htslib, which has no microsoft visual studio / CMake support.

# Local assembly
- [ ] MMTYPE descriptor broken, not decodable in reference software
- [X] Read names
- [X] Quality values
- [ ] QV depth > 1
- [ ] Mapping score depth > 1
- [ ] Splices
- [ ] Secondary alignments
- [X] Class P
- [X] Class N
- [X] Class M
- [X] Class I
- [ ] Class HM
- [X] Softclips
- [X] Hardclips
- [X] Paired reads
- [X] Unpaired reads
- [X] CI Tests

# Reference based compression
- [ ] Not yet started
