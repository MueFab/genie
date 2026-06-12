# [TODO]: Implement the JBIG Decoder Interface and Document Codecs

## Goal
Implement a dedicated `jbig::JBIGDecoder` class wrapper, isolate decoding from `jbig::JBIGEncoder`, refactor the calling modules to use the new decoder, and document the GABAC and general codec interfaces.

## Context
Currently, `jbig::JBIGEncoder` contains a `decode` method, which violates the single-responsibility principle and splits interface boundaries. A stub `jbig::JBIGDecoder` exists under `src/genie/entropy/jbig/decoder.h` but is incomplete (lacks `std::vector` support and is not used in genotype/contact coders). Furthermore, the user wishes to document GABAC's encoder/decoder interface and investigate if the GABAC/other codec APIs apply for generalized entropy coding.

## Technical Strategy
1. **Define JBIGDecoder Interface**:
   - Provide stringstream & vector overloads:
     - `void decode(std::stringstream& input, std::stringstream& output, uint32_t& ncols, uint32_t& nrows);`
     - `void decode(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, uint32_t& ncols, uint32_t& nrows);`
2. **Remove decode from JBIGEncoder**: Remove the `decode` function from `jbig::JBIGEncoder` declarations and implementation.
3. **Refactor Coder Modules**:
   - Update `contact_coder.cc` to use `jbig::JBIGDecoder` instead of `jbig::JBIGEncoder` for tile decompression.
   - Update `genotype_coder.cc` to use `jbig::JBIGDecoder` instead of `jbig::JBIGEncoder` for binary matrix decompression.
4. **Update Unit Tests**:
   - Update `test/entropy/jbig_test.cc` to verify round-trips via `jbig::JBIGDecoder`.
5. **Document GABAC & Codec Interfaces**:
   - Review and update `codec_interface_analysis.md` to detail GABAC's and other codecs' interfaces and evaluate the unified C++ API.

## Task Breakdown
| Step | Task | Action | Status | Dependencies |
| :--- | :--- | :--- | :--- | :--- |
| **A** | Implement `jbig::JBIGDecoder` | `[x]` Add stringstream and vector overloads in `decoder.h`/`decoder.cc` | Completed | - |
| **B** | Clean `jbig::JBIGEncoder` | `[x]` Remove `decode` declarations/implementations from `encoder.h`/`encoder.cc` | Completed | A |
| **C** | Refactor genotype/contact coders | `[x]` Instantiate and use `jbig::JBIGDecoder` in coders | Completed | A, B |
| **D** | Update `jbig_test.cc` | `[x]` Replace `encoder.decode()` with `decoder.decode()` in test cases | Completed | C |
| **E** | Document Codecs Interface | `[x]` Validate and finalize `codec_interface_analysis.md` | Completed | - |
| **F** | Verify Build & Tests | `[x]` Compile and pass all unit tests | Completed | D |

## Parallel Development Plan
- **Track 1:** Step A, B, C, D, F (Code implementation, refactoring, and verification)
- **Track 2:** Step E (Documentation and interface analysis)

## Dependencies
- Step A -> Step B -> Step C -> Step D -> Step F

## Definition of Done
- [ ] `jbig::JBIGDecoder` implements stringstream and vector decompression overloads.
- [ ] `jbig::JBIGEncoder` has no `decode` methods.
- [ ] Genotype and contact coders decode JBIG using `jbig::JBIGDecoder`.
- [ ] `jbig_test.cc` tests decoding with `jbig::JBIGDecoder`.
- [ ] GABAC and general codec interfaces are documented.
- [ ] All unit tests compile and pass.

## Debugging Experience
| Problem | Failed Solution | Successful Solution |
| :--- | :--- | :--- |

## Technical Debt / Future Work
- None.
