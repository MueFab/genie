# Refactoring Plan: `core::Writer` → `util::BitWriter`

## Goal
Replace all `core::Writer` usage with `util::BitWriter` directly (like develop), while keeping all part6 features and new modules.

## Current State
- `core::Writer` exists in part6 as wrapper around `util::BitWriter` (dual-overload pattern)
- 86+ files still reference `core::Writer`
- Develop uses `util::BitWriter` directly

## Principle
**One writer type**: Use only `util::BitWriter` (like develop) - no `core::Writer` wrapper.

---

## Refactoring Order (Dependency-Based)

### Phase 1: Core Infrastructure (Base Layer)
Files that OTHER code depends on - refactor these FIRST.

| Order | File | Reason |
|-------|------|--------|
| 1 | `core/writer.h` | Remove `core::Writer` class entirely |
| 2 | `core/array_type.h/.cc` | Has dual `toFile()` overloads for both Writer types |
| 3 | `util/bit_writer.h/.cc` | Ensure BitWriter has all needed methods |

### Phase 2: Parameter/Annotation Classes
These are consumed by higher-level annotation code.

| Order | File | Change Required |
|-------|------|-----------------|
| 4 | `core/parameter/annotation/algorithm_parameters.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 5 | `core/parameter/annotation/compressor_parameter_set.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 6 | `core/parameter/annotation/descriptor_configuration.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 7 | `core/parameter/annotation/contact_matrix_parameters.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 8 | `core/parameter/annotation/annotation_encoding_parameters.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 9 | `core/parameter/annotation/attribute_parameter_set.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 10 | `core/parameter/annotation/tile_configuration.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 11 | `core/parameter/annotation/tile_structure.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 12 | `core/parameter/annotation/record.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |

### Phase 3: Access Unit/Annotation Classes
Built on top of parameter classes.

| Order | File | Change Required |
|-------|------|-----------------|
| 13 | `core/access_unit/annotation/annotation_access_unit_header.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 14 | `core/access_unit/annotation/block_header.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 15 | `core/access_unit/annotation/block.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 16 | `core/access_unit/annotation/block_payload.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 17 | `core/access_unit/annotation/record.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 18 | `core/access_unit/annotation/typed_data.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |

### Phase 4: Record Classes
Various record types that write themselves.

| Order | File | Change Required |
|-------|------|-----------------|
| 19 | `core/record/contact/record.h/.cc` | `Write(core::Writer&)` → `Write(util::BitWriter&)` |
| 20 | `core/record/variant/record.h/.cc` | `Write(core::Writer&)` → `Write(util::BitWriter&)` |
| 21 | `core/record/site/record.h/.cc` | `Write(core::Writer&)` → `Write(util::BitWriter&)` |
| 22 | `core/record/data_unit/record.h/.cc` | `Write(core::Writer&)` → `Write(util::BitWriter&)` |
| 23 | `core/sample_record/record.h/.cc` | `Write(core::Writer&)` → `Write(util::BitWriter&)` |
| 24 | `core/feature_record/record.h/.cc` | `Write(core::Writer&)` → `Write(util::BitWriter&)` |
| 25 | `core/functional_annotation_record/record.h/.cc` | `Write(core::Writer&)` → `Write(util::BitWriter&)` |
| 26 | `core/gene_expression_record/record.h/.cc` | `Write(core::Writer&)` → `Write(util::BitWriter&)` |
| 27 | `core/track_property_record/record.h/.cc` | `Write(core::Writer&)` → `Write(util::BitWriter&)` |
| 28 | `core/linked_record/linked_record.h/.cc` | `Write(core::Writer&)` → `Write(util::BitWriter&)` |

### Phase 5: Genotype/Likelihood/Contact
Coders that use Writers.

| Order | File | Change Required |
|-------|------|-----------------|
| 29 | `genotype/genotype_parameters.h/.cc` | `Write(core::Writer&)` → `Write(util::BitWriter&)` |
| 30 | `contact/contact_coder.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |
| 31 | `likelihood/likelihood_coder.h/.cc` | `write(core::Writer&)` → `write(util::BitWriter&)` |

### Phase 6: High-Level Modules & Apps
Top-level consumers.

| Order | File | Change Required |
|-------|------|-----------------|
| 32 | `annotation/annotation.h/.cc` | Uses `core::Writer` for log writing |
| 33 | `variantsite/variantsite_parser.h` | Uses `core::Writer` for fieldWriter/attrWriter |
| 34 | `apps/genie/annotation/code.cc` | Uses `core::Writer` for dataUnitWriter |

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

## Decision: What to do with core::Writer?

**Option A**: Keep `core::Writer` as IS (no changes) - simpler
**Option B**: Remove `core::Writer` entirely, migrate everything to `util::BitWriter`

**Recommendation**: Option B (like develop) for consistency.

---

**Awaiting confirmation.**