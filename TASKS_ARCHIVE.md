# Task Index (Archive)

| Task ID | Date | Description | Status |
|:---:|:---:|:---|:---:|
| 2026-01-21-cross-backend-tests | 2026-01-21 | Cross-Backend Verification (Golden Master) | Done |
| 2026-01-21-verify-genotype-roundtrip | 2026-01-21 | Genotype JBIG Round-Trip & Integration Tests | Done |
| 2026-01-21-extend-genotype-tests | 2026-01-21 | Extend Genotype Tests for ZSTD and BSC Codecs | Done |
| 2026-01-21-implement-eigen-backend | 2026-01-21 | Implement Eigen Backend for Genotype Module | Done |
| 2026-01-21-implement-std-backend | 2026-01-21 | Implement STD Backend for Genotype Module | Done |
| 2026-01-21-refactor-genotype-backends | 2026-01-21 | Refactor Genotype Module & Unify Backend Flags | Done |
| 2026-01-22-port-genotype-changes-from-temp | 2026-01-22 | Port Genotype Changes from temp_part6 | Done |
| 2026-01-22-finalize-temp-integration | 2026-01-22 | Finalize temp_part6 Integration | Done |
| 2026-01-22-implement-likelihood-bitwriter | 2026-01-22 | Implement BitWriter/BitReader for LikelihoodPayload | Done |
| 2026-01-22-refactor-likelihood-backends | 2026-01-22 | Refactor Likelihood Module for Multi-Backend Support | Done |
| 2026-01-22-fix-likelihood-compression-segfaults | 2026-01-22 | Fix Likelihood Compression and Segmentation Faults | Done |
| 2026-04-27-fix-likelihood-integrity | 2026-04-27 | Fix Likelihood Entropy Coding & Bitstream Integrity | Done |
| 2026-04-15-refactor-backend-dispatchers | 2026-04-15 | Genomic Backend Dispatchers | Done |
| 2026-04-17-standardize-tests-final | 2026-04-17 | Finalize Test Suite Standardization | Done |
| 2026-04-27-repo-health-check | 2026-04-27 | Initial Repository Health Check | Done |
| 2026-04-15-unify-contact-coder | 2026-04-15 | Unify Contact Module Coder | Done |
| 2026-04-15-unify-genotype-coder | 2026-04-15 | Unify Genotype Module Coder | Done |
| 2026-04-27-optimize-append-arr-element | 2026-04-27 | Optimize append_arr_element in Contact Coder (O(n²) → O(n)) | Done |
| 2026-04-27-optimize-contact-loop | 2026-04-27 | Optimize contact loop (remove dead code, cache sizes) | Done |
| 2026-06-05-genotype-merge-develop-part6-with-temp-part6 | 2026-06-05 | Merge develop-part6 with origin/temp_part6 | Done |
| 2026-06-05-merge-develop-with-develop-part6 | 2026-06-05 | Merge develop with develop-part6 | Done |
| 2026-06-09-revert-bitwriter-to-develop | 2026-06-09 | Revert BitWriter and Utilities to origin/develop Standard | Done |
| 2026-04-27-merge-develop-part6-to-develop | 2026-04-27 | Merge develop-part6 to develop branch | Done |
| 2026-04-28-common-attribute-field | 2026-04-28 | Standardize AttributeField - unify all parser field types | Done |
| 2026.04.28-core-writer-refactor | 2026-04-28 | Refactor core/writer dependencies | Done |
| 2026.04.28-annotation-parser-integration | 2026-04-28 | Integrate all annotation parsers into annotation.cc | Done |
| 2026.04.28-annot-integration-incorporate-new-annotations | 2026-04-28 | Add new annotation types to the build system | Done |
| 2026-06-12-reorganize-docs | 2026-06-12 | Reorganize Documentation Hub (Category-level INDEX.md sub-folders) | Done |
| 2026-06-12-entropy-jbig-decoder-interface | 2026-06-12 | Implement JBIG decoder interface and document GABAC/codec interfaces | Done |
| 2026-06-12-harmonize-jbig-files | 2026-06-12 | Harmonize JBIG C implementation wrapper files | Done |
| 2026-06-12-standardize-zstd-wrapper | 2026-06-12 | Standardize ZSTD wrapper class and decouple decoder | Done |
| 2026-06-12-comprehensive-entropy-tests | 2026-06-12 | Create comprehensive unit tests and crash-proof JBIG decoder | Done |
| 2026-06-12-migrate-to-genie-entropy | 2026-06-12 | Migrate from thirdparty/codecs to native genie/entropy C++ wrappers | Done |
| 2026-06-15-standardize-find-or-fetch-dependencies | 2026-06-15 | Standardize Find-or-Fetch dependency resolution scripts | Done |
| 2026-04-27-parallelize-contact-tiles | 2026-04-27 | Parallelize contact tile encoding with OpenMP | Done |
| 2026-08-11-cleanup-duplicate-hierarchies | 2026-08-11 | Grand plan: resolve temp_part6 dedup/divergence (variantsite, access_unit, variant rename) | Done |
| 2026-08-11-track-a-dedup-variantsite | 2026-08-11 | Delete src/genie/variantsite (redundant w/ annotation) | Done |
| 2026-08-11-track-b-dedup-access-unit-hierarchy | 2026-08-11 | Delete old core/record/annotation_access_unit (keep core/access_unit/annotation) | Done |
| 2026-08-11-track-c-rename-variant-to-genotype | 2026-08-11 | Rename core/record/variant -> core/record/genotype | Done |
| 2026-08-11-track-d-integrate-verify | 2026-08-11 | Prune core::Writer + merge A/B/C + full verify | Done |
| 2026-04-29-deprecate-core-writer | 2026-04-29 | Deprecate core::Writer and add comprehensive bitstream tests | Done |
| 2026.08.25-main-fix_warnings_wuninitialized | 2026.08.25 | Fix -Wuninitialized in AnnotationAccessUnitHeader::read | Done |
| 2026.08.25-main-fix_warnings_mechanical | 2026.08.25 | Fix mechanical compiler warnings | Done |
| 2026.08.28-main-library_rename_genie_variant_site | 2026.08.28 | Phase 1.5: rename library genie-annotation -> genie-variant-site | Done |
| 2026.08.28-main-cli_orchestrator_migration | 2026.08.28 | Phase 2: modernize ParameterSetComposer and prune legacy PSC | Done |
| 2026.08.28-main-build_consolidation_tmp_builds | 2026.08.28 | Consolidate all build directories into genie/tmp/builds/* | Done |
| 2026.08.25-main-cli_dependency_audit | 2026.08.25 | Audit apps/genie/ CLI dependencies and external libraries | Done |
| 2026-04-29-delete-core-writer | 2026-04-29 | Remove deprecated core::Writer and migrate remaining tests to util::BitWriter | Done |





