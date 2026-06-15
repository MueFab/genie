# [TODO]: Create Comprehensive Unit Tests and Crash-Proof JBIG Decoder

## Goal
Crash-proof the legacy JBIG C library interface by removing process-terminating `exit(1)` calls, and implement a robust suite of comprehensive unit tests for both the JBIG and ZSTD entropy codec wrappers.

## Context
While basic round-trip tests exist for both JBIG and ZSTD, they do not test error boundaries, empty payloads, corrupted streams, or scaling with large files. Crucially, the legacy C code in `jbig_interface.c` calls `exit(1)` when decoding corrupted data, which abruptly terminates the entire process (crashing any caller including the test suite). Resolving this process-termination bug is a prerequisite to testing corrupted streams safely.

## Technical Strategy
1. **Crash-Proof JBIG Interface (`src/genie/entropy/jbig/jbig_interface.c`)**:
   - Replace all `exit(1)` statements in `mpegg_jbig_compress` and `mpegg_jbig_decompress` with resource cleanup and clean error returns.
   - Correct the invalid `dest == NULL` check on line 210 after `realloc` to check `*dest == NULL` instead.
   - Refactor `JBIGDecoder::decode` in `decoder.cc` to abort early and clean up if the C decompression returns an error.
2. **ZSTD Unit Test Expansion (`test/entropy/zstd_test.cc`)**:
   - **Empty Inputs:** Verify behavior when passing empty strings/vectors to encoder/decoder.
   - **Corrupted Inputs:** Pass random/garbage data and truncated frames to `ZSTDDecoder`, validating error detection without crashes.
   - **Large Payload:** Round-trip a 1MB buffer of highly-compressible text and purely random bytes.
3. **JBIG Unit Test Expansion (`test/entropy/jbig_test.cc`)**:
   - **Empty Dimensions:** Verify behavior when dimensions (ncols, nrows) are zero or buffers are empty.
   - **Dimension Mismatches:** Test encoder and decoder checks when the input buffer size does not match the specified width and height.
   - **Corrupted Streams:** Pass garbage/truncated data to `JBIGDecoder` and verify it fails gracefully.
   - **Large Payload:** Round-trip a large matrix (e.g., 2048 x 1024) of structured data.

## Task Breakdown
| Step | Task | Action | Status | Dependencies |
| :--- | :--- | :--- | :--- | :--- |
| **A** | Crash-proof JBIG Interface | [ ] Refactor `jbig_interface.c` to replace `exit(1)` with error returns and fix realloc checks | Pending | - |
| **B** | Expand ZSTD Tests | [ ] Implement ZSTDEmptyInputTest, ZSTDCorruptedInputTest, and ZSTDLargePayloadTest | Pending | - |
| **C** | Expand JBIG Tests | [ ] Implement JBIGEmptyInputTest, JBIGDimensionMismatchTest, JBIGCorruptedInputTest, and JBIGLargePayloadTest | Pending | A |
| **D** | Verify Build & Test | [ ] Compile and pass all unit tests with mamba run | Pending | B, C |

## Parallel Development Plan
- **Track 1:** Step A -> Step C (JBIG crash-proofing and tests)
- **Track 2:** Step B (ZSTD tests)

## Dependencies
- Step A -> Step C
- Step B, Step C -> Step D

## Definition of Done
- [ ] No `exit(1)` calls exist in `jbig_interface.c`.
- [ ] ZSTD unit tests cover empty inputs, corrupted inputs, and large payloads.
- [ ] JBIG unit tests cover zero/empty dimensions, mismatched dimensions, corrupted inputs, and large payloads.
- [ ] No segmentation faults or process crashes occur under malformed inputs.
- [ ] All unit, integration, and conformance tests compile and pass.

## Debugging Experience
| Problem | Failed Solution | Successful Solution |
| :--- | :--- | :--- |

## Technical Debt / Future Work
- None.
