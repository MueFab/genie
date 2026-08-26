# Task Index

| Task ID | Date | Description | Status |
|:---:|:---:|:---|:---:|
| 2026-08-11-cleanup-duplicate-hierarchies | 2026-08-11 | Grand plan: resolve temp_part6 dedup/divergence (variantsite, access_unit, variant rename) | Done |
| 2026-08-11-track-a-dedup-variantsite | 2026-08-11 | Delete src/genie/variantsite (redundant w/ annotation) | Done |
| 2026-08-11-track-b-dedup-access-unit-hierarchy | 2026-08-11 | Delete old core/record/annotation_access_unit (keep core/access_unit/annotation) | Done |
| 2026-08-11-track-c-rename-variant-to-genotype | 2026-08-11 | Rename core/record/variant -> core/record/genotype | Done |
| 2026-08-11-track-d-integrate-verify | 2026-08-11 | Prune core::Writer + merge A/B/C + full verify | Done (core::Writer prune deferred — see 2026-04-29-deprecate-core-writer) |
| 2026-04-29-deprecate-core-writer | 2026-04-29 | Deprecate core::Writer (Doxygen @deprecated) and add comprehensive util::BitReader/BitWriter tests | Done |
| 2026.08.25-main-fix_warnings_wuninitialized | 2026.08.25 | Fix real bug: -Wuninitialized x2 in AnnotationAccessUnitHeader::read | Done |
| 2026.08.25-main-fix_warnings_mechanical | 2026.08.25 | Quick mechanical: -Wcast-user-defined/-Wsign-compare/-Wconversion/-Wunused-result (11 sites) | Done |