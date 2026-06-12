# [TODO]: Standardize ZSTD Wrapper Class and Decouple Decoder

## Goal
Decouple `zstd::ZSTDEncoder` and `zstd::ZSTDDecoder`, replace legacy custom C-bridge functions (`mpegg_zstd_`) with direct, standard `<zstd.h>` API calls, and refactor the genotype coder and tests to use the standardized C++ wrapper classes.

## Context
Currently, `zstd::ZSTDEncoder` contains both compression and decompression methods, which violates single-responsibility principles. The implementation relies on legacy C bridge functions (`mpegg_zstd_compress` and `mpegg_zstd_decompress`) compiled into `mpeggCodecs-static`. Standardizing ZSTD with clean `ZSTDEncoder` and `ZSTDDecoder` wrappers calling system `zstd` APIs directly allows us to cleanly remove the legacy bridge code.

## Technical Strategy
1. **Declare ZSTDDecoder**: Declare `ZSTDDecoder` in `src/genie/entropy/zstd/decoder.h` with:
   - `void decode(std::stringstream &input, std::stringstream &output);`
   - `void decode(const std::vector<uint8_t> &input, std::vector<uint8_t> &output);`
2. **Remove decode from ZSTDEncoder**: Remove `decode` declarations/implementations from `encoder.h`/`encoder.cc` and add vector overload to `ZSTDEncoder::encode`.
3. **Use standard ZSTD APIs**:
   - `ZSTDEncoder::encode` will call standard `ZSTD_compressBound` and `ZSTD_compress`.
   - `ZSTDDecoder::decode` will call `ZSTD_getFrameContentSize` and `ZSTD_decompress`.
4. **Refactor genotype coder**: Update `genotype_coder.cc` to use `zstd::ZSTDDecoder` instead of calling `mpegg_zstd_decompress`.
5. **Update Unit Tests**: Update `test/entropy/zstd_test.cc` to verify round-trips via `zstd::ZSTDDecoder`.

## Task Breakdown
| Step | Task | Action | Status | Dependencies |
| :--- | :--- | :--- | :--- | :--- |
| **A** | Implement `zstd::ZSTDDecoder` | `[x]` Add `ZSTDDecoder` class declarations & implementations | Completed | - |
| **B** | Clean `zstd::ZSTDEncoder` | `[x]` Remove `decode` method and add vector `encode` overload | Completed | A |
| **C** | Refactor genotype coder | `[x]` Instantiate and use `zstd::ZSTDDecoder` and `zstd::ZSTDEncoder` in `genotype_coder.cc` | Completed | A, B |
| **D** | Update `zstd_test.cc` | `[x]` Replace `encoder.decode()` with `decoder.decode()` and test vector overloads | Completed | C |
| **E** | Verify Build & Tests | `[x]` Compile and pass all unit tests | Completed | D |

## Parallel Development Plan
- None (linear workflow).

## Dependencies
- Step A -> Step B -> Step C -> Step D -> Step E

## Definition of Done
- [x] `zstd::ZSTDDecoder` is implemented with stringstream and vector overloads.
- [x] `zstd::ZSTDEncoder` has no `decode` methods.
- [x] ZSTD wrappers call standard `<zstd.h>` functions directly.
- [x] Genotype coder uses `zstd::ZSTDDecoder`.
- [x] All unit and integration tests compile and pass.

## Debugging Experience
| Problem | Failed Solution | Successful Solution |
| :--- | :--- | :--- |

## Technical Debt / Future Work
- None.
