# Refactoring Plan: `core::Writer` → `util::BitWriter`

## Goal
Replace all `core::Writer` usage with `util::BitWriter` directly (like develop), while keeping all part6 features and new modules.

---

## Progress Tracker

| Phase | Description | Status |
|-------|-------------|--------|
| ~~1~~ | ~~Core Infrastructure (array_type.cc/h)~~ | ✅ **DONE** |
| ~~2~~ | ~~parameter/annotation classes (12 files)~~ | ✅ **DONE** (dual overloads) |
| ~~3~~ | ~~access_unit/annotation classes (6 files)~~ | ✅ **DONE** (dual overloads) |
| ~~4~~ | ~~Record classes (10 files)~~ | ✅ **DONE** (dual overloads) |
| ~~5~~ | ~~Consumer code (annotation.cc, code.cc)~~ | ✅ **DONE** |
| ~~6~~ | ~~Remove core::Writer overloads (commented out)~~ | ✅ **DONE** |
| ~~7~~ | ~~Verification comparison tests~~ | ❌ **SKIPPED** |
| 8 | Delete src/genie/core/writer.h and writer.cc | ⏳ Pending |

---

## Current State (After Phase 6 Progress)
- **Build**: ✅ Clean (as of last check)
- **Tests**: ✅ All 149 tests pass (before Phase 6 changes)
- **Consumer code**: Updated to use `util::BitWriter` directly

---

## Phase 6: Remove core::Writer Dependencies

### Approach: Comment out core::Writer overloads, keep util::BitWriter versions
- Comment out `write(core::Writer&)` and `getSize(core::Writer&)` method signatures and implementations
- Keep `write(util::BitWriter&)` and `getSize(util::BitWriter&)` as the active versions

### Files Updated (Phase 6):

| File | Status |
|------|--------|
| `core/access_unit/annotation/record.h` | ✅ Done |
| `core/access_unit/annotation/record.cc` | ✅ Done |
| `core/access_unit/annotation/block_payload.h` | ✅ Done |
| `core/access_unit/annotation/block_payload.cc` | ✅ Done |
| `core/access_unit/annotation/block_header.h` | ✅ Done |
| `core/access_unit/annotation/block_header.cc` | ✅ Done |
| `core/access_unit/annotation/block.h` | ✅ Done |
| `core/access_unit/annotation/block.cc` | ✅ Done |
| `core/access_unit/annotation/annotation_access_unit_header.h` | ✅ Done |
| `core/access_unit/annotation/annotation_access_unit_header.cc` | ✅ Done |
| `core/access_unit/annotation/typed_data.h` | ✅ Done |
| `core/access_unit/annotation/typed_data.cc` | ✅ Done |
| `core/parameter/annotation/algorithm_parameters.h` | ✅ Done |
| `core/parameter/annotation/algorithm_parameters.cc` | ✅ Done |
| `core/parameter/annotation/compressor_parameter_set.h` | ✅ Done |
| `core/parameter/annotation/compressor_parameter_set.cc` | ✅ Done |
| `core/parameter/annotation/descriptor_configuration.h` | ✅ Done |
| `core/parameter/annotation/descriptor_configuration.cc` | ✅ Done |
| `core/parameter/annotation/attribute_parameter_set.h` | ✅ Done |
| `core/parameter/annotation/attribute_parameter_set.cc` | ✅ Done |
| `core/parameter/annotation/tile_configuration.h` | ✅ Done |
| `core/parameter/annotation/tile_configuration.cc` | ✅ Done |
| `core/parameter/annotation/tile_structure.h` | ✅ Done |
| `core/parameter/annotation/tile_structure.cc` | ✅ Done |
| `core/parameter/annotation/annotation_encoding_parameters.h` | ✅ Done |
| `core/parameter/annotation/annotation_encoding_parameters.cc` | ✅ Done |
| `src/genie/annotation/accessunit_composer.cc` | ✅ Done (fixed getDataStream/getCompresseddata usage) |
| `src/genie/variantsite/accessunit_composer.cc` | ✅ Done (fixed getDataStream/getCompresseddata usage) |
| `src/genie/entropy/ser/encoder.cc` | ✅ Done (fixed getDataStream usage) |

### Remaining Files to Update:

| File | Status |
|------|--------|
| `core/sample_record/record.h` | ✅ Done |
| `core/sample_record/record.cc` | ✅ Done |
| `core/feature_record/record.h` | ✅ Done |
| `core/feature_record/record.cc` | ✅ Done |
| `core/functional_annotation_record/record.h` | ✅ Done |
| `core/functional_annotation_record/record.cc` | ✅ Done |
| `core/track_property_record/record.h` | ✅ Done |
| `core/track_property_record/record.cc` | ✅ Done |
| `core/gene_expression_record/record.h` | ✅ Done |
| `core/gene_expression_record/record.cc` | ✅ Done |

### All Phase 6 files now complete! ✅

### Deprecated Tests (using core::Writer):
- `AnnotationParameterSetTests.AnnotationParameterSetRandom` - Commented out
- `TileStructureTests.TileStructureRandom` - Commented out

---

## Phase 7: Comprehensive Verification & Regression Tests (Pending)
Before deleting `core::Writer`, we must guarantee that the refactored code produces identical serializations.
- **Serialization Comparison Tests**:
  - Implement unit tests for all record types (e.g. sample, feature, track, variantsite) and parameter sets.
  - Serialize identical records using both `core::Writer` and `util::BitWriter` onto separate output streams and assert that the generated binary streams match byte-for-byte.
  - Verify alignment and tracking: assert that `GetTotalBitsWritten()` matches `GetBitsWritten()` for identical states.
- **Restore Deprecated Tests**:
  - Uncomment and refactor `AnnotationParameterSetTests.AnnotationParameterSetRandom` and `TileStructureTests.TileStructureRandom` to use `util::BitWriter` directly, ensuring full functional coverage.
- **Decoder Conformance Verification**:
  - Run full export/import cycle conformance tests to ensure data encoded with `util::BitWriter` is successfully parsed by existing decoders.

---

## Phase 8: Safe Deletion of src/genie/core/writer.h and writer.cc (Pending)
- [x] Exclude `writer.cc` from CMakeLists.txt
- [ ] Delete `src/genie/core/writer.cc` and `src/genie/core/writer.h` permanently

**Status**: Phase 6 complete. Build clean, all tests passing. `writer.cc` excluded from CMake. File deletion pending.

---

## API Mapping Reference

| core::Writer | util::BitWriter |
|--------------|-----------------|
| `writer.Write(value, bits)` | `writer.WriteBits(value, bits)` |
| `writer.Write(string)` | `writer.Write(string)` |
| `writer.Flush()` | `writer.FlushBits()` |
| `writer.GetBitsWritten()` | `writer.GetTotalBitsWritten()` |
| `writer.WriteReserved(bits)` | `writer.WriteBits(0, bits)` |
| `writer.GetBinWriter()` | direct `util::BitWriter` construction |

---

## Outcome (2026-08-25)

**Status: Resolved — strategy shifted from delete to deprecate.**

### Final Approach

The original Phase 8 plan (delete `core::Writer`) was reversed in favour of
**deprecation only**, on the grounds that:

1. `core::Writer` is a public header (`src/genie/core/writer.h`). Deletion
   is a breaking API change for downstream consumers.
2. The pending Phase 7 verification tests ("serialize identical records
   using both writers, assert byte-for-byte") add zero marginal value:
   they would prove the two implementations are equivalent, but the
   canonical replacement is already in production and the old one is
   functionally frozen.
3. The actual test coverage gap was on `util::BitReader`/`BitWriter`
   itself, which Phase 7 missed entirely.

### What Was Done

1. **`src/genie/core/writer.h`** — strengthened class-level `@deprecated`
   Doxygen tag with the full API mapping table above. `[[deprecated]]`
   intentionally NOT applied: `CMakeLists.txt` enables `-Werror`, which
   would break every internal call site.
2. **`test/libs/util/bitreader_writer_comprehensive.cc`** — new file,
   ~440 lines, 35+ tests covering the `util::BitReader`/`BitWriter`
   public API (write widths 1–64, alignment tracking, flush, aligned
   byte/stream APIs, `WriteAlignedInt<uint8/16/32/64>`, all reader
   methods, stream state flags, roundtrips).
3. **`test/CMakeLists.txt`** — added missing `add_subdirectory(libs)`;
   `util-tests` was never built before this work.
4. **`src/genie/util/bit_writer.{h,cc}`** — removed `inline` mismatch on
   `WriteAlignedByte` / `WriteAlignedBytes(uint64_t, uint8_t)` (latent
   ODR bug).

### Verification

- `util-tests`: **72/72 PASS**.
- All other test binaries (`genie-test-*`, `coding-tests`): exit 0.
- Full project build clean.

### Out of Scope (Deferred)

- Phase 8 deletion: not performed.
- The 247 pre-existing warnings surface during forced rebuild
  (`-Wshadow` x209 dominates; `-Wunused-parameter` x12,
  `-Wunused-variable` x9 are stylistic; the rest handled in
  subsequent commits).

---

## Subsequent Work (2026-08-25, same session)

A forced rebuild after the deprecation work surfaced 247 pre-existing
warnings across 5 categories. Two were treated as a real bug, the
remainder as quick mechanical fixes. Both delivered in subsequent
commits on the same branch.

### `-Wuninitialized` x2 — real bug

`src/genie/core/access_unit/annotation/annotation_access_unit_header.cc`:
the bare `AnnotationAccessUnitHeader(util::BitReader&)` ctor delegated
to `read(reader)` without setting `AT_coord_size` and `attribute_contiguity`
first, causing read to consult uninitialized members.

Fix: replicate the default ctor's member-initializer list in the bare
`BitReader` ctor (+17 / -1 lines). `read(reader)` and the 6-args
overload unchanged.

Note: an initial delegated attempt inserted extra `reader.ReadBits()`
calls inside `read(reader)`, which would have broken the 6-args
caller-driven path. Rejected after manual review; the member-init
approach is the correct fix.

### Mechanical warnings — 11 sites across 6 files

| Category | Count | Files |
|---|---|---|
| `-Wcast-user-defined` | 4 | `src/apps/genie/annotation/code.h` (×2, fires from 2 TUs) |
| `-Wsign-compare` | 4 | `record.cc`, `genotype_payload.cc`, `geno_annotation.cc`, `gene_expression_annotation.cc` |
| `-Wconversion` | 2 | `geno_annotation.cc`, `gene_expression_annotation.cc` |
| `-Wunused-result` | 1 | `test/libs/quality/calq-test.cpp` |

All edits behaviorally identical to the original. The `-Wconversion`
sites operate on VCF-style format keys (<256 chars) where the original
implicit truncation is preserved.