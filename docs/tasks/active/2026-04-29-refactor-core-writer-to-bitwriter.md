# Refactoring Plan: `core::Writer` → `util::BitWriter`

## Goal
Replace all `core::Writer` usage with `util::BitWriter` directly (like develop), while keeping all part6 features and new modules.

## Strategy: Incremental Migration with Dual Overloads
- Each class keeps BOTH `write(core::Writer&)` and `write(util::BitWriter&)` methods during transition
- This allows gradual migration without breaking existing code
- Consumer code uses implicit conversion from `core::Writer` to `util::BitWriter`

---

## Progress Tracker

| Phase | Description | Status |
|-------|-------------|--------|
| ~~1~~ | ~~Core Infrastructure (array_type.cc/h)~~ | ✅ **DONE** |
| ~~2~~ | ~~parameter/annotation classes (12 files)~~ | ✅ **DONE** (dual overloads) |
| ~~3~~ | ~~access_unit/annotation classes (6 files)~~ | ✅ **DONE** (dual overloads) |
| ~~4~~ | ~~Record classes (10 files)~~ | ✅ **DONE** (dual overloads) |
| ~~5~~ | ~~Consumer code (implicit conversion)~~ | ✅ **DONE** |
| **6** | **Remove core::Writer entirely** | 🔄 **IN PROGRESS** |

---

## Current State (After Phase 5)
- **Build**: ✅ Clean
- **Tests**: ✅ All 149 tests pass
- **Dual overloads**: All annotation/record classes have both `write(core::Writer&)` and `write(util::BitWriter&)`
- **Implicit conversion**: `core::Writer` has `operator util::BitWriter&()` enabling seamless interop

---

## Phase 6: Remove core::Writer (CURRENT)

### Files still referencing core::Writer (46 occurrences):
- Implementation method signatures (kept for dual-overload compat)
- Log writers in `annotation.cc`, `code.cc`

### Remaining Work:
1. ✅ Update documentation
2. ⏳ Update `src/genie/annotation/annotation.cc` to use BitWriter for txtwriter
3. ⏳ Update `src/apps/genie/annotation/code.cc` to use BitWriter
4. ⏳ Remove `write(core::Writer&)` overloads from all classes
5. ⏳ Delete `src/genie/core/writer.h`
6. ⏳ Full build and test verification

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

**Status**: Phase 6 in progress.