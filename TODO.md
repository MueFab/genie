# TODO

## Current


FOLDERS & FILES TO RE-WORK NEXT
doc/
resources/ -> data/
source/    -> src/
tests/     -> test/
           -> thirdparty/ (from source/libs/third-party/)
TODO.md


- [ ] make script util/boilerplate.sh

- [ ] add ./ci/run-* to .travis.yml
- [ ] add ./ci/report-coverage.sh to .travis.yml
- [ ] gabac-app: rename cl options to sth like --input-file instead of --input_file
- [ ] genie: rename cl options to --output-file etc. instead of --output-file-path
- [ ] Fix compilation warnings
- [ ] Use libs from newest C++ standards (e.g. std::filesystem)
- [ ] Resolve FIXMEs and TODOs
- [ ] Resolve clang-tidy warnings
- [ ] Check whether both SamRecord and MinSamRecord classes are needed
- [ ] Check comments
  - [ ] Sentences should start with an uppercase letter and end with a full stop.
  - [ ] Statements should start with an uppercase letter and not end with a full stop.

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
- [ ] Check everything with CLion
- [ ] Logger class
- [ ] Exceptions
- [ ] Rule of 5/6
- [ ] Clean up exceptions (use only simple derived class - w/o all the special things)
- [ ] u(32) issue in GABAC
- [ ] Format log and error messages uniformly
- [ ] Check for correct usage of assertions versus exceptions
- [ ] Follow the C++ Core Guidelines (https://isocpp.github.io/CppCoreGuidelines), in particular C.21
- [ ] Re-structure: cmake-build-\*/, doc/, thirdparty/, src/, test/, data/
- [ ] Check everything with Valgrind
- [ ] Update README.md once everything is working
- [ ] TNT email -> jTODO.md
- [ ] u(32) issues
- [ ] Doxyfile diff
- [ ] Travis
- [ ] todo.sh
- [ ] Check LICENSE
- [ ] Unify I/O
  - [ ] Bitstream R/W should be done using HM's bitstream classes adapted for 64 bits
  - [ ] "Ordinary" file R/W (for e.g. SPRING's temporary files) should be done using the C++ standard library
- [ ] Social media image (on GitHub)
- [ ] Virtual dtor in all classes that something is being derived from (use 'override' in derived classes)
- [ ] Python interfaces
  - [ ] Only use numpy arrays
  - [ ] Just as the example "c-and-python"


- [ ] Update README.md once everything is working


Shubham
making Genie's SPRING mode MPEG-G conformant
write a parameter set and one access unit - which only contains e.g. the encoded read IDs - to a file using the functionality from the format library (see e.g. https://github.com/mitogen/genie/blob/feature/ureads-encoder/source/apps/ureads-encoder/encoding.cc on the feature/ureads-encoder branch). Try to decode that with the reference software.

Junaid
investigate gabac roundtrip
check adaptive CABAC

Fabian
local assembly

Daniel/Jannis
Part 1

## SPRING integration

### encoding_parameters()

- dataset_type = 0
- alphabet_ID = 0
- read_length = 0 (we use rlen)
- number_of_template_segments_minus1 = 1 (single-/paired-end)
- max_au_data_unit_size = 0 (leave unspecified)
- pos_40_bits_flag = 1 (just to be save)
- qv_depth = 0 or 1
- as_depth = 0
- num_classes = 1
- class_ID[0] = 6
- class_specific_dec_cfg_flag[i] = 0
- num_groups = 0
- multiple_alignments_flag = 0
- spliced_reads_flag = 0
- multiple_signature_base = 0
- qv_coding_mode = 1
- qvps_flag = 0
- qvps_preset_ID = 0
- qv_reverse_flag = 1
- crps_flag = 1

#### parameter_set_crps()

- cr_alg_ID = 4 (global assembly)

### Notes

- In the AU header, to get the reads_count, the easiest way is the size of the rcomp descriptor stream (subseq_vector[1][0].size()). num_blocks is the number of descriptors used, i.e., number of values first index of subseq_vector takes + 1 each for quality and id.
- To put the compressed block payloads into AUs, the easiest way would be write the compressed read descriptors to temporary files in generate-read-streams.cc, then write the compressed quality and id descriptors to temporary files in reorder-compress-quality-id.cc. Finally you can have a separate combining step to combine these into the AU structure.
- Note that the vector tokens[i][j][] in spring corresponds to vector decoded_tokens[i*16+j][] in the standard (e.g., section 10.4.19).
- For token type we need to be a bit careful. The decoded_tokens is treated as a byte array in the standard.  Depending on the value of j, we either extract 1 byte from this array (when j = 0, 1, 2, 3, 5) or we extract a 4 byte integer from this (when j = 4). In my tokens array, everything is int64, with value within 255 when j = 0, 1, 2, 3, 5 and value within 2^32-1 when j = 4. So we should store things with correct number of bytes.
- For quality values, we’ll need to subtract 33 from the ASCII value before compression, since the decompression process in 7.3.2.2.1.1 adds 33 after compression.
- While the global assembly in the reference decoder gets implemented, we can start testing the ureads_flag in spring which allows us to test the read id tokentype compression.
- As we put things in the conformant format, we should also update the spring decompressor. Since the spring decompressor has additional capabilities like matching pairs, it will be good to keep it working and not rely solely on the reference decoder.
- Spring uses BBHash as a hash function (boo-phf.h) which uses fopen. Just wanted to let you know since Jan asked me about any file handling other than std::fstream. This is someone else’s code and I have not touched it at all. Feel free to change it to fstream if needed.
