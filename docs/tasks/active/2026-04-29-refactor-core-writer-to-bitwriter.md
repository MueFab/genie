# Refactoring Plan: `core::Writer` → `util::BitWriter`

## Goal
Replace all `core::Writer` usage with `util::BitWriter` directly (like develop), while keeping all part6 features and new modules.

## Strategy: Incremental Migration with Dual Overloads
- Each class keeps BOTH `write(core::Writer&)` and `write(util::BitWriter&)` methods during transition
- This allows gradual migration without breaking existing code
- When all consumers migrate, the `core::Writer` version can be removed

## Current State
- Build: ✅ Clean
- Tests: ✅ All 149 tests pass

---

## Progress Tracker

| Phase | Description | Status |
|-------|-------------|--------|
| 1 | Core Infrastructure (array_type.cc/h) | ✅ Complete |
| 2 | parameter/annotation classes (12 files) | 🔄 In Progress |
| 3 | access_unit/annotation classes (6 files) | ⏳ Pending |
| 4 | Record classes (10 files) | ⏳ Pending |
| 5 | Genotype/Likelihood/Contact coders | ⏳ Pending |
| 6 | High-level modules & apps | ⏳ Pending |
| 7 | Remove core::Writer entirely | ⏳ Pending |

---

## Phase 2: parameter/annotation Classes

All files in this phase already have BOTH `write(core::Writer&)` and `write(util::BitWriter&)` methods:

| File | core::Writer Version | util::BitWriter Version |
|------|---------------------|------------------------|
| `algorithm_parameters.h/.cc` | ✅ exists | ✅ exists |
| `compressor_parameter_set.h/.cc` | ✅ exists | ✅ exists |
| `descriptor_configuration.h/.cc` | ✅ exists | ✅ exists |
| `contact_matrix_parameters.h/.cc` | ✅ exists | ✅ exists |
| `annotation_encoding_parameters.h/.cc` | ✅ exists | ✅ exists |
| `attribute_parameter_set.h/.cc` | ✅ exists | ✅ exists |
| `tile_configuration.h/.cc` | ✅ exists | ✅ exists |
| `tile_structure.h/.cc` | ✅ exists | ✅ exists |
| `record.h/.cc` | ✅ exists | ✅ exists |

**Note**: All Phase 2 files already have dual overloads - no changes needed!

---

## Phase 3: access_unit/annotation Classes

| File | core::Writer Version | util::BitWriter Version |
|------|---------------------|------------------------|
| `annotation_access_unit_header.h/.cc` | ✅ exists | ✅ exists |
| `block_header.h/.cc` | ✅ exists | ✅ exists |
| `block.h/.cc` | ✅ exists | ✅ exists |
| `block_payload.h/.cc` | ✅ exists | ✅ exists |
| `record.h/.cc` | ✅ exists | ✅ exists |
| `typed_data.h/.cc` | ✅ exists | ✅ exists |

**Note**: All Phase 3 files already have dual overloads - no changes needed!

---

## Phase 4: Record Classes

| File | core::Writer Version | util::BitWriter Version |
|------|---------------------|------------------------|
| `record/contact/record.h/.cc` | ✅ exists | ✅ exists |
| `record/variant/record.h/.cc` | ✅ exists | ✅ exists |
| `record/site/record.h/.cc` | ✅ exists | ✅ exists |
| `record/data_unit/record.h/.cc` | ✅ exists | ✅ exists |
| `sample_record/record.h/.cc` | ✅ exists | ✅ exists |
| `feature_record/record.h/.cc` | ✅ exists | ✅ exists |
| `functional_annotation_record/record.h/.cc` | ✅ exists | ✅ exists |
| `gene_expression_record/record.h/.cc` | ✅ exists | ✅ exists |
| `track_property_record/record.h/.cc` | ✅ exists | ✅ exists |
| `linked_record/linked_record.h/.cc` | ✅ exists | ✅ exists |

---

## Phase 5-6: Coders, Modules & Apps

These are the files that **consume** the above classes. They need to be updated to call the `util::BitWriter` version instead of `core::Writer`.

### Files needing updates (to call BitWriter version instead of Writer):

| Order | File | Current Call | Needs Change |
|-------|------|--------------|--------------|
| 1 | `genotype/genotype_parameters.cc` | `write(core::Writer&)` | Update to call `write(util::BitWriter&)` |
| 2 | `contact/contact_coder.cc` | `write(core::Writer&)` | Update to call `write(util::BitWriter&)` |
| 3 | `likelihood/likelihood_coder.cc` | `write(core::Writer&)` | Update to call `write(util::BitWriter&)` |
| 4 | `annotation/annotation.cc` | `core::Writer` | Update to `util::BitWriter` |
| 5 | `variantsite/variantsite_parser.h` | `core::Writer` | Update to `util::BitWriter` |
| 6 | `apps/genie/annotation/code.cc` | `core::Writer` | Update to `util::BitWriter` |

---

## Migration Pattern

### Before (with core::Writer):
```cpp
void write(core::Writer& writer) const {
    writer.Write(value, 8);
    writer.Flush();
}
```

### After (with util::BitWriter):
```cpp
void write(util::BitWriter& writer) const {
    writer.WriteBits(value, 8);
    writer.FlushBits();
}
```

### API Mapping Table

| core::Writer | util::BitWriter |
|--------------|-----------------|
| `writer.Write(value, bits)` | `writer.WriteBits(value, bits)` |
| `writer.Write(string)` | `writer.Write(string)` (exists in both) |
| `writer.Flush()` | `writer.FlushBits()` |
| `writer.GetBitsWritten()` | `writer.GetTotalBitsWritten()` |
| `writer.WriteReserved(bits)` | `writer.WriteBits(0, bits)` |
| `writer.Write(istream*)` | Use `writer.WriteAlignedStream(*in)` or loop |

---

## Verification Steps

After each phase:
1. Build: `make -j4`
2. Test: Run relevant test binary
3. Full test: All 13 test binaries pass

---

## Final: Remove core::Writer

Once all consumers are migrated to use `util::BitWriter`:
1. Remove `core::Writer` class from `src/genie/core/writer.h`
2. Update any remaining references
3. Full build and test verification

---

**Status**: Ready to proceed with Phase 5 updates (consumer files).