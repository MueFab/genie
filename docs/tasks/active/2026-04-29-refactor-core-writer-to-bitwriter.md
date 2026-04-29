# Refactoring Plan: `core::Writer` → `util::BitWriter`

## Goal
Replace all `core::Writer` usage with `util::BitWriter` directly (like develop), while keeping all part6 features and new modules.

## Strategy: Incremental Migration with Dual Overloads
- Each class keeps BOTH `write(core::Writer&)` and `write(util::BitWriter&)` methods during transition
- This allows gradual migration without breaking existing code
- Consumer code uses implicit conversion from `core::Writer` to `util::BitWriter`

## Current State
- Build: ✅ Clean
- Tests: ✅ All 149 tests pass

---

## Progress Tracker

| Phase | Description | Status |
|-------|-------------|--------|
| 1 | Core Infrastructure (array_type.cc/h) | ✅ Complete |
| 2 | parameter/annotation classes (12 files) | ✅ Complete (dual overloads) |
| 3 | access_unit/annotation classes (6 files) | ✅ Complete (dual overloads) |
| 4 | Record classes (10 files) | ✅ Complete (dual overloads) |
| 5 | Consumer code (implicit conversion) | ✅ Complete |
| **6** | **Remove core::Writer entirely** | ⏳ **NEXT** |

---

## Phase 6: Remove core::Writer

### Files to Update (remove core::Writer references):

| File | Action |
|------|--------|
| `src/genie/core/writer.h` | Delete entire file (or keep empty for ABI compatibility) |
| `src/genie/annotation/annotation.cc` | Replace `genie::core::Writer txtwriter(&txtfile, true)` with `genie::util::BitWriter` |
| `src/apps/genie/annotation/code.cc` | Replace `genie::core::Writer` with `genie::util::BitWriter` |
| `src/apps/genie/annotation/code.cc` | Replace `txtWriter.Flush()` with `txtWriter.FlushBits()` |

### Verification Steps:
1. Build: `make -j4`
2. Test: Run all 13 test binaries
3. Verify no `core::Writer` references remain

---

## API Mapping Reference

| core::Writer | util::BitWriter |
|--------------|-----------------|
| `writer.Write(value, bits)` | `writer.WriteBits(value, bits)` |
| `writer.Write(string)` | `writer.Write(string)` |
| `writer.Flush()` | `writer.FlushBits()` |
| `writer.GetBitsWritten()` | `writer.GetTotalBitsWritten()` |
| `writer.WriteReserved(bits)` | `writer.WriteBits(0, bits)` |

---

**Status**: Ready to proceed with Phase 6 (remove core::Writer).