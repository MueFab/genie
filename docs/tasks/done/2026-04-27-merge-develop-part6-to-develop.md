# [MERGE PLAN]: develop-part6 → develop (Detailed Analysis)

## Goal
Merge `develop-part6` branch into `develop` with minimal disruption, using a 12-step phased approach based on dependency hierarchy.

## Updated Merge Strategy (Post-BitWriter Reversion)

Now that the **`BitWriter` reversion is complete**, we have successfully eliminated the custom pointer-based constructor/bypass conflicts in `util`. This significantly reduces the risk of structural conflicts in downstream files (like records and test cases) during the merge.

The refined strategy for the next step of merging `develop-part6` and `develop` is:

### Step 0: Git History Alignment (Prerequisite)
Since `develop-part6` and `develop` have divergent/unrelated Git histories, we must create a shared base before starting the phased merge:
1. Create local backup bundles of both branches.
2. Checkout a new merge-working branch off `develop`.
3. Fetch `develop-part6` and merge it with `--no-commit` to align history and resolve any baseline file conflicts (e.g. root `.gitignore` or `README.md`).

### Step 1: Phased Module-by-Module Merge Plan
We will merge the codebase in order of dependency flow:

#### Phase 1: Base Libraries & New Backend (Steps 1–3)
* **`genie-util` & `genie-module`**: Merge these base layers. The utility layer will now compile cleanly since `BitWriter` matches upstream `develop`.
* **`genie-backend`**: Import this new module (~10 files). It is 100% new code providing the Eigen/xtensor dispatch logic, so it has no direct overlap with `develop`.

#### Phase 2: Codecs & File Formats (Steps 4–5)
* **`genie-entropy` (ZSTD Adaptation)**:
  * **Critical action**: Adapt `develop-part6`'s standalone `ZSTDEncoder` to use the `develop` `EntropyEncoder` and `EntropyDecoder` base classes.
  * Restore `decoder.cc`/`param_decoder.cc` from `develop` and preserve `develop`'s external `find_package(Zstd)` pattern rather than `part6`'s static thirdparty build.
* **`genie-format` (MGB & FASTA Renames)**:
  * Run `git mv` for renamed files to preserve history:
    * `data_unit_factory.cc/h` $\rightarrow$ `data-unit-factory.cc/h` (MGB)
    * `fai_file.cc`/`fasta_source.cc` $\rightarrow$ `fai-file.cc`/`fasta-source.cc` (FASTA)
  * Ingest the new MGG subdirectories (`annotation_access_unit/`, `annotation_table/`, and `dataset_parameterset/`).

#### Phase 3: Core Restructuring & Coder Unification (Steps 6–9)
* **`genie-core` Restructuring**:
  * Restructure `genie-core` to the hierarchical directory model (under `access_unit/annotation/` and `parameter/annotation/`).
  * Add the new core record types (`variant/`, `site/`, `contact/`, `data_unit/`, `linked_record/`).
* **Unify Coders**: Remove backend-specific files (e.g., `contact_coder_std.cc`, `contact_coder_eigen.cc`, `contact_coder_xtensor.cc`) and replace them with the unified coders that route through the backend dispatcher.

#### Phase 4: High-Level Annotation & Verification (Steps 10–12)
* **`genie-annotation`**: Merge the new annotation module (100% new, ~30 files) and link it.
* **Build Matrix & Verification**: Rebuild the root CMakeLists.txt and verify all test suites across all 9 backend combinations (Contact/Genotype/Likelihood $\times$ {STD, Eigen, XTensor}).

### Self-Criticism & Risks
* **ZSTD Divergence**: The ZSTD codec implementation in `develop-part6` deviates significantly from the inheritance architecture of other codecs in `develop`. Step 4 will require manual rewriting of the ZSTD I/O layer.
* **Circular Dependency Risk**: `genie-core` now relies on `genie-backend`, which relies on `genie-util`. We must verify the linker doesn't complain about circular inclusion paths during Step 6.

### Relationship with Other Active Tasks

The following table maps other active tasks in the repository to their relevant merge phases and steps:

| Task ID & Link | Description | Relationship to Merge | Target Merge Phase/Step |
| :--- | :--- | :--- | :--- |
| [2026-04-29-refactor-core-writer-to-bitwriter.md](file:///home/adhisant/workspace/genie-part6/docs/tasks/active/2026-04-29-refactor-core-writer-to-bitwriter.md) | Migrate from `core::Writer` to `util::BitWriter` and remove deprecated `core::Writer` completely. | **Post-Merge Cleanup**: Removal of `core/writer.h` (Phase 7 of the task) requires all modules to be successfully merged and compiled first. | Step 11/12 (Verification) / Post-Merge |
| [2026.04.28-common_attribute_field_unification.md](file:///home/adhisant/workspace/genie-part6/docs/tasks/active/2026.04.28-common_attribute_field_unification.md) | Standardize `AttributeField` structure to unify all annotation parser field types. | **Restructuring Prerequisite / Integration Core**: Must be integrated as part of `genie-core` (Step 6) and `genie-annotation` (Step 10) to avoid compiler/linker errors. | Step 6 (Merge `genie-core`) & Step 10 (Add `genie-annotation`) |
| [2026.04.28-annotation_parser_integration.md](file:///home/adhisant/workspace/genie-part6/docs/tasks/active/2026.04.28-annotation_parser_integration.md) | Integrate all annotation parsers into `annotation.cc`. Currently blocked by `Attributes::add()` type mismatch. | **Blocking Core Integration**: Relies on the `AttributeField` unification task. Needs to be executed as part of `genie-annotation` module integration. | Step 10 (Add `genie-annotation`) |
| [2026.04.28-annot_integration_incorporate_new_annotations.md](file:///home/adhisant/workspace/genie-part6/docs/tasks/active/2026.04.28-annot_integration_incorporate_new_annotations.md) | Add new annotation types to build system and fix include paths. | **Module-level Integration**: Executed during Phase 4 of the merge to bring in GTF, BED, track, and track property annotations. | Step 10 (Add `genie-annotation`) |
| [2026.04.28-core_writer_refactor.md](file:///home/adhisant/workspace/genie-part6/docs/tasks/active/2026.04.28-core_writer_refactor.md) | Refactor `core/writer` dependencies (superseded by bitwriter migration). | **Obsolete**: Superseded by the direct `BitWriter` migration task. To be archived. | N/A (Archived) |

## Context
The `develop-part6` branch introduces:
- **New modules**: annotation, backend, variantsite
- **Unified coders**: contact, genotype, likelihood (replacing 3-backend-per-module)
- **Core restructuring**: access_unit reorganization, new record types
- **Thirdparty additions**: zstd codec, nlohmann json, filesystem headers

---

## 0. Pre-Merge: Create Shared History (CRITICAL - DO FIRST)

**Problem**: `develop-part6` and `develop` have **unrelated histories** (no common ancestor), preventing direct merge.

**Solution**: Forward-port `develop` commits onto `develop-part6` to create shared base.

### Step 0: Setup & Backup
```bash
# Create backup bundles
cd /home/adhisant/workspace/genie-develop
git bundle create /tmp/develop-backup.bundle develop

cd /home/adhisant/workspace/genie-part6
git bundle create /tmp/part6-backup.bundle develop-part6

# Verify backups
git bundle verify /tmp/develop-backup.bundle
git bundle verify /tmp/part6-backup.bundle
```

### Step 0b: Forward-Port Develop to develop-part6
```bash
# Switch to develop-part6
cd /home/adhisant/workspace/genie-part6
git checkout develop-part6

# Cherry-pick or merge develop's commits onto develop-part6
# Option A: Cherry-pick each commit (preserves history)
git cherry-pick <develop-commit-hash>

# Option B: Merge develop into develop-part6 (faster)
git fetch origin develop
git merge origin/develop --no-commit
# Resolve conflicts (use develop's version for conflicting files)
git add .
git commit -m "merge: forward-port develop to develop-part6"

# Push updated develop-part6
git push origin develop-part6
```

**Result**: `develop-part6` now contains all of develop's code AND its own new features, with shared history.

**Status**: Completed (Verified Locally)

---

## 1. Dependency Architecture

```mermaid
flowchart TD
    subgraph "External Dependencies"
        XTENSOR[xtensor]
        EIGEN[Eigen3]
        ZSTD[ZSTD codec]
        LZMA[LZMA codec]
        JBIG[JBIG codec]
        BSC[BSC codec]
        JSON["nlohmann/json"]
        FS[filesystem]
    end

    subgraph "Infrastructure"
        UTIL["genie-util<br/>~45 files<br/>LOW DIFF"]
    end

    subgraph "Core System"
        CORE["genie-core<br/>~150 files<br/>HIGH DIFF"]
    end

    subgraph "Backend Abstraction [NEW]"
        BACKEND["genie-backend<br/>10 files<br/>100% NEW"]
    end

    subgraph "Codec Modules"
        ENTROPY["genie-entropy<br/>~120 files<br/>MEDIUM DIFF"]
    end

    subgraph "Format Handlers"
        FORMAT["genie-format<br/>~210 files<br/>MEDIUM DIFF"]
    end

    subgraph "High-Level Modules [NEW/REFACTORED]"
        CONTACT["genie-contact<br/>~20 files<br/>UNIFIED"]
        GENOTYPE["genie-genotype<br/>~12 files<br/>UNIFIED"]
        LIKELIHOOD["genie-likelihood<br/>~10 files<br/>UNIFIED"]
        ANNOTATION["genie-annotation<br/>~30 files<br/>100% NEW"]
    end

    UTIL --> CORE
    UTIL --> BACKEND
    UTIL --> ENTROPY

    CORE --> BACKEND
    CORE --> ENTROPY
    CORE --> FORMAT
    CORE --> CONTACT
    CORE --> GENOTYPE
    CORE --> LIKELIHOOD

    BACKEND --> CONTACT
    BACKEND --> GENOTYPE
    BACKEND --> LIKELIHOOD

    ENTROPY --> GENOTYPE
    ENTROPY --> ANNOTATION
    CORE --> ANNOTATION

    CONTACT --> ANNOTATION
    GENOTYPE --> ANNOTATION
    LIKELIHOOD --> ANNOTATION

    FORMAT --> ANNOTATION
    CORE --> ANNOTATION
```

---

## 2. Module Dependency

### Dependency Flow Chart (✅ = Completed, 🔄 = In Progress, ⏳ = Pending)

```mermaid
flowchart TB
    subgraph "External Dependencies"
        XTENSOR["xtensor<br/>(optional)"]
        EIGEN["Eigen3<br/>(optional)"]
        ZSTD["ZSTD codec"]
        LZMA["LZMA codec"]
        JBIG["JBIG codec"]
        BSC["BSC codec"]
        JSON["nlohmann/json"]
        FS["filesystem"]
    end

    subgraph "genie-util" #90EE90
        UTIL["✅ genie-util<br/>~45 files<br/>Risk: LOW"]
    end

    subgraph "genie-backend" #90EE90
        BACKEND["✅ genie-backend<br/>~10 files<br/>100% NEW<br/>Risk: MEDIUM"]
    end

    subgraph "genie-core" #FFB6C1
        CORE["✅ genie-core<br/>~150 files<br/>Risk: HIGH"]
    end

    subgraph "genie-entropy" #FFFF99
        ENTROPY["✅ genie-entropy<br/>~120 files<br/>Risk: MEDIUM"]
    end

    subgraph "genie-format" #FFFF99
        FORMAT["✅ genie-format<br/>~210 files<br/>Risk: MEDIUM"]
    end

    subgraph "genie-module" #90EE90
        MOD["✅ genie-module<br/>~7 files<br/>Risk: LOW"]
    end

    subgraph "genie-name" #90EE90
        NAME["✅ genie-name<br/>~2 files<br/>Risk: LOW"]
    end

    subgraph "genie-quality" #90EE90
        QUAL["✅ genie-quality<br/>~20 files<br/>Risk: LOW"]
    end

    subgraph "genie-read" #90EE90
        READ["✅ genie-read<br/>~40 files<br/>Risk: LOW"]
    end

    subgraph "genie-contact" #FFFF99
        CONTACT["✅ genie-contact<br/>~20 files<br/>UNIFIED<br/>Risk: MEDIUM"]
    end

    subgraph "genie-genotype" #FFFF99
        GENO["✅ genie-genotype<br/>~12 files<br/>UNIFIED<br/>Risk: MEDIUM"]
    end

    subgraph "genie-likelihood" #FFFF99
        LIKE["✅ genie-likelihood<br/>~10 files<br/>UNIFIED<br/>Risk: MEDIUM"]
    end

    subgraph "genie-annotation" #FFB6C1
        ANNOT["✅ genie-annotation<br/>~30 files<br/>100% NEW<br/>Risk: HIGH"]
    end

    %% External dependencies
    XTENSOR -.-> BACKEND
    EIGEN -.-> BACKEND
    ZSTD -.-> ENTROPY
    LZMA -.-> ENTROPY
    JBIG -.-> ENTROPY
    BSC -.-> ENTROPY

    %% Base layer (✅ done)
    UTIL --> CORE
    UTIL --> BACKEND
    UTIL --> ENTROPY
    UTIL --> FORMAT

    %% Core dependencies
    CORE --> BACKEND
    CORE --> ENTROPY
    CORE --> FORMAT
    CORE --> MOD
    CORE --> NAME
    CORE --> QUAL
    CORE --> READ
    CORE --> CONTACT
    CORE --> GENO
    CORE --> LIKE

    %% Backend dependencies
    BACKEND --> CONTACT
    BACKEND --> GENO
    BACKEND --> LIKE

    %% Format dependencies
    FORMAT --> READ

    %% High-level modules
    ENTROPY --> GENO
    ENTROPY --> ANNOT
    CORE --> ANNOT
    CONTACT --> ANNOT
    GENO --> ANNOT
    LIKE --> ANNOT

    %% Legend with status
    classDef HIGH risk fill:#FFB6C1,stroke:#FF0000,stroke-width:3px
    classDef MEDIUM risk fill:#FFFF99,stroke:#FFA500,stroke-width:2px
    classDef LOW risk fill:#90EE90,stroke:#228B22,stroke-width:1px
    classDef DONE stroke:#228B22,stroke-width:2px,fill:#90EE90

    class UTIL,MOD,NAME,QUAL,READ LOW
    class BACKEND,ENTROPY,FORMAT,CONTACT,GENO,LIKE MEDIUM
    class CORE,ANNOT HIGH
```

### Internal Structure of genie-core

```mermaid
flowchart TB
    subgraph "genie-core Internal Architecture"
        direction TB

        subgraph "Core Infrastructure"
            API["api.cc/h"]
            C_API["c_api.cc/h"]
            CLASSIFIER["classifier.h<br/>classifier_bypass.cc/h<br/>classifier_regroup.cc/h"]
            FLOW["flow_graph.cc/h<br/>flow_graph_*.cc"]
        end

        subgraph "Access Units"
            AU["access_unit.cc/h<br/>(FLAT in develop)"]
            AU_HIER["access_unit/<br/>access_unit.cc/h<br/>(HIERARCHICAL in part6)"]
            ANNOT_AU["access_unit/annotation/<br/>7 files<br/>NEW in part6"]
        end

        subgraph "Parameters"
            PARAM["parameter/<br/>descriptor/descriptor_present/<br/>computed_ref.cc/h"]
            ANNOT_PARAM["parameter/annotation/<br/>8 files<br/>NEW in part6"]
        end

        subgraph "Records (part6 additions)"
            VARIANT_REC["record/variant/<br/>2 files<br/>NEW"]
            LINKED_REC["linked_record/<br/>1 file<br/>NEW"]
            SITE_REC["record/site/<br/>1 file<br/>NEW"]
            DATA_UNIT_REC["record/data_unit/<br/>1 file<br/>NEW"]
            CONTACT_REC["record/contact/<br/>1 file<br/>NEW"]
        end

        subgraph "Utility Files"
            ARRAY["array_type.cc/h<br/>NEW in part6<br/>Dual Writer support"]
            NDARRAY["ndarray.cc/h"]
            WRITER["writer.cc/h<br/>part6 only"]
        end

        subgraph "Format Export/Import"
            FORMAT_EXP["format_exporter.cc/h<br/>format_exporter_compressed.cc/h"]
            FORMAT_IMP["format_importer.cc/h<br/>format_importer_compressed.cc/h<br/>format_importer_null.cc/h"]
        end

        subgraph "Meta"
            META["meta/access_unit.cc/h<br/>meta/block_header/"]
        end
    end

    %% Styling
    classDef NEW fill:#DDA0DD,stroke:#800080,stroke-width:2px
    classDef PART6_ONLY fill:#E6E6FA,stroke:#9370DB,stroke-width:1px,stroke-dash:5,5

    class ANNOT_AU,ANNOT_PARAM NEW
    class VARIANT_REC,LINKED_REC,SITE_REC,DATA_UNIT_REC,CONTACT_REC NEW
    class ARRAY,WRITER,AU_HIER PART6_ONLY
```

### genie-core Internal Legend

| Style | Meaning | Examples |
|-------|---------|----------|
| **Solid purple border** | NEW in part6 (didn't exist in develop) | `access_unit/annotation/`, `parameter/annotation/` |
| **Dashed purple border** | part6-only file (not in develop) | `array_type.cc/h`, `writer.cc/h`, hierarchical `access_unit/` |
| **Flat structure** | Files that exist in both branches | `api.cc/h`, `classifier*.cc`, `flow_graph*.cc` |

---

| Color | Risk Level | Modules |
|-------|-------------|---------|
| 🟢 `#90EE90` | **LOW** | genie-util, genie-module, genie-name, genie-quality, genie-read |
| 🟡 `#FFFF99` | **MEDIUM** | genie-backend, genie-entropy, genie-format, genie-contact, genie-genotype, genie-likelihood |
| 🔴 `#FFB6C1` | **HIGH** | genie-core, genie-annotation |

### Module Summary Table

| Module | Type | Files | Status | Risk | Key Dependencies |
|--------|------|-------|--------|------|------------------|
| **genie-util** | EXISTING | ~45 | Unchanged | LOW | None (base) |
| **genie-core** | EXISTING | ~150 | Modified | HIGH | util, backend (NEW) |
| **genie-backend** | NEW | 10 | 100% New | MEDIUM | util, xtensor, eigen |
| **genie-entropy** | EXISTING | ~120 | Modified | MEDIUM | util, core, zstd, lzma, jbig, bsc |
| **genie-format** | EXISTING | ~210 | Modified | MEDIUM | util, core |
| **genie-module** | EXISTING | ~7 | Modified | LOW | core |
| **genie-name** | EXISTING | ~2 | Unchanged | LOW | core |
| **genie-quality** | EXISTING | ~20 | Unchanged | LOW | core |
| **genie-read** | EXISTING | ~40 | Unchanged | LOW | core, format |
| **genie-contact** | UNIFIED | ~20 | Refactored | MEDIUM | core, backend, util |
| **genie-genotype** | UNIFIED | ~12 | Refactored | MEDIUM | core, backend, util |
| **genie-likelihood** | UNIFIED | ~10 | Refactored | MEDIUM | core, backend, util |
| **genie-annotation** | NEW | ~30 | 100% New | HIGH | core, contact, genotype, likelihood, entropy |

---

## 3. Merge Execution Steps

### Step 0: Pre-Merge Preparation
- [x] Create backup of both branches
- [x] Verify develop is up-to-date with remote
- [x] Document current state of both repositories
- **Status**: Completed (Verified Locally)

---

### Step 1: Merge genie-util
- **Rationale**: 45 files, mostly deletions of unused quantizers, no conflicts expected
- **Action**:
  ```bash
  git checkout develop
  git merge develop-part6 --no-commit
  # Accept "theirs" for: src/genie/util/*
  git add src/genie/util/*
  git commit -m "merge(genie-util): part6 changes"
  ```
- **Risk**: LOW
- **Success Criteria**: genie-util compiles without errors
- **Status**: Completed (Verified Locally)

---

### Step 2: Merge genie-module
- **Rationale**: 7 files, minor changes, isolated from other modules
- **Action**:
  ```bash
  git merge develop-part6 --no-commit
  # Accept "theirs" for: src/genie/module/*
  git add src/genie/module/*
  git commit -m "merge(genie-module): part6 changes"
  ```
- **Risk**: LOW
- **Success Criteria**: genie-module compiles without errors
- **Status**: Completed (Verified Locally)

---

### Step 3: Add genie-backend (NEW MODULE)
- **Rationale**: 10 files, 100% new, no develop equivalent
- **Action**:
  ```bash
  git add src/genie/backend/
  git commit -m "feat(backend): add new backend abstraction layer"
  ```
- **Risk**: MEDIUM
- **Success Criteria**: genie-backend compiles with optional xtensor/eigen
- **Status**: Completed (Verified Locally)

---

### Step 4: Merge genie-entropy
- **Rationale**: 120 files, encoder/decoder API updates
- **Key Changes**:
  - `zstd/subsequence.cc` → `zstd/subsequence.cpp` (rename)
  - ZSTD encoder/decoder API completely rewritten in part6
  - BSC, LZMA, JBIG may have minor updates
  - **IMPORTANT**: Use develop's codec structure (external `find_package(Zstd)`) NOT part6's thirdparty approach
- **Action**:
  ```bash
  git merge develop-part6 --no-commit
  # Manual merge for ZSTD API changes
  # Use develop codec structure (find_package) for ZSTD, NOT mpeggCodecs-static
  # Track file rename for subsequence.cc → .cpp
  git mv src/genie/entropy/zstd/subsequence.cc src/genie/entropy/zstd/subsequence.cpp || true
  # Keep decoder.cc, param_decoder.cc from develop (part6 removed them)
  git add src/genie/entropy/
  git commit -m "merge(genie-entropy): zstd, bsc, lzma, jbig"
  ```
- **Risk**: MEDIUM
- **Success Criteria**: All entropy codecs encode/decode roundtrip passes
- **Status**: Completed (Verified Locally)

---

## 4a. Entropy Codec Structure Decision

| Aspect | develop | develop-part6 | Decision |
|--------|---------|---------------|----------|
| ZSTD Source | External `find_package(Zstd)` | Internal `mpeggCodecs-static` | **Use develop (external)** |
| ZSTD Decoder | Present | **Removed** from sources | **Restore from develop** |
| Encoder Class | `Encoder : EntropyEncoder` | `ZSTDEncoder` (standalone) | **Adapt to develop pattern** |
| Decoder Class | `Decoder : EntropyDecoder` | Missing | **Restore from develop** |
| BSC Source | External libbsc | Internal `mpeggCodecs-static` | **Use develop pattern** |
| Linking | PUBLIC | PRIVATE | **Align to develop** |

### Step 5: Merge genie-format
- **Rationale**: 210 files, significant restructuring in mgb/mgg/sam
- **Key Conflicts**:
  - `mgb/data_unit_factory.cc` → `mgb/data-unit-factory.cc` (rename with hyphen)
  - MGG multiple naming inconsistencies
  - SAM files removed/reorganized
- **Action**:
  ```bash
  git merge develop-part6 --no-commit
  # Manual merge focusing on mgb/, mgg/ directories
  # Prefer part6 for new code, develop for existing APIs
  git add src/genie/format/
  git commit -m "merge(genie-format): mgb, mgg, mgg, sam"
  ```
- **Risk**: HIGH
- **Success Criteria**: Format modules read/write cycle passes
- **Status**: Completed (Verified Locally)

---

### Step 6: Merge genie-core (CRITICAL)
- **Rationale**: ~150 files, CMakeLists completely different structure
- **Critical Conflicts**:
  1. `CMakeLists.txt`: Develop is flat list, part6 is hierarchical
  2. New subdirectories in part6: `access_unit/annotation/`, `linked_record/`, `parameter/annotation/`
  3. New files: `array_type.cc/h`, `ndarray.cc/h`, `writer.cc/h`
- **Action**:
  ```bash
  git merge develop-part6 --no-commit
  # Strategy: Use develop CMakeLists as base, add part6 subdirectories
  # Merge subdirectories one at a time
  git add src/genie/core/
  git commit -m "merge(genie-core): full restructure"
  ```
- **Risk**: CRITICAL
- **Success Criteria**: genie-core tests pass for all 9 backend combinations
- **Status**: Completed (Verified Locally)

---

### Step 7: Merge genie-contact
- **Rationale**: Unified from 3 backend-specific files
- **Action**:
  ```bash
  # Remove old: contact_coder_std.cc/h, contact_coder_eigen.cc/h, contact_coder_xtensor.cc/h
  # Add new: contact_coder.cc/h (unified with backend dispatcher)
  git add src/genie/contact/
  git commit -m "refactor(contact): unified backend dispatcher"
  ```
- **Risk**: MEDIUM
- **Success Criteria**: Contact Golden Master tests pass
- **Status**: Completed (Verified Locally)

---

### Step 8: Merge genie-genotype
- **Rationale**: Unified from 3 backend-specific files
- **Action**:
  ```bash
  # Remove old: genotype_coder_std.cc/h, genotype_coder_eigen.cc/h, genotype_coder_xtensor.cc/h
  # Add new: genotype_coder.cc/h (unified with backend dispatcher)
  git add src/genie/genotype/
  git commit -m "refactor(genotype): unified backend dispatcher"
  ```
- **Risk**: MEDIUM
- **Success Criteria**: Genotype Golden Master tests pass
- **Status**: Completed (Verified Locally)

---

### Step 9: Merge genie-likelihood
- **Rationale**: Unified from 3 backend-specific files
- **Action**:
  ```bash
  # Remove old: likelihood_coder_std.cc/h, likelihood_coder_eigen.cc/h, likelihood_coder_xtensor.cc/h
  # Add new: likelihood_coder.cc/h (unified with backend dispatcher)
  git add src/genie/likelihood/
  git commit -m "refactor(likelihood): unified backend dispatcher"
  ```
- **Risk**: MEDIUM
- **Success Criteria**: Likelihood Golden Master tests pass
- **Status**: Completed (Verified Locally)

---

### Step 10: Add genie-annotation (NEW MODULE)
- **Rationale**: 30 files, 100% new code
- **Action**:
  ```bash
  git add src/genie/annotation/
  # Update src/genie/CMakeLists.txt to add annotation
  git add src/genie/CMakeLists.txt
  git commit -m "feat(annotation): add new annotation module"
  ```
- **Risk**: LOW
- **Success Criteria**: genie-annotation compiles standalone
- **Status**: Completed (Verified Locally)

---

### Step 11: Finalize Root CMakeLists
- **Rationale**: Must add new modules and update existing entries
- **Conflicts**:
  - `add_subdirectory(annotation)` - NEW
  - `add_subdirectory(backend)` - NEW
  - `add_subdirectory(contact)` - existing, needs update
  - `add_subdirectory(genotype)` - existing, needs update
  - `add_subdirectory(likelihood)` - existing, needs update
  - Thirdparty: nlohmann, filesystem, zstd additions
- **Action**:
  ```bash
  # Manual merge of root CMakeLists.txt
  # Add new thirdparty dependencies
  git add CMakeLists.txt thirdparty/
  git commit -m "merge: update build system for part6 modules"
  ```
- **Risk**: HIGH
- **Success Criteria**: Full project compiles
- **Status**: Completed (Verified Locally)

---

### Step 12: Full Build Verification
- **Action**:
  ```bash
  # Run 9-combination build matrix
  ./verify_matrix.sh

  # Execute all test suites
  ctest --output-on-failure

  # Run Golden Master tests
  bin/genie-test-likelihood --gtest_filter=*GoldenMaster*
  bin/genie-test-contact --gtest_filter=*GoldenMaster*
  bin/genie-test-genotype --gtest_filter=*GoldenMaster*
  ```
- **Risk**: HIGH
- **Success Criteria**:
  - All 9 combinations build successfully
  - All tests pass
  - 9-combination matrix green
- **Status**: Completed (Verified Locally)

---

## 4. Risk Mitigation

| Step | Risk Level | Mitigation Strategy |
|------|------------|---------------------|
| 0 | LOW | Full backup before starting |
| 1-2 | LOW | Straight merge, easy rollback |
| 3 | MEDIUM | Isolated module, easy rollback |
| 4 | MEDIUM | Check ZSTD API compatibility first |
| 5 | HIGH | Manual merge, test after each subdir |
| 6 | CRITICAL | Incremental commits, test after each addition |
| 7-9 | MEDIUM | Unified coders are self-contained |
| 10 | LOW | Pure new addition |
| 11 | HIGH | Manual CMakeLists merge |
| 12 | HIGH | Full test matrix |

---

## 5. Definition of Done

- [x] All 12 steps completed without rollback
- [x] All 9 build combinations pass
- [x] All test suites pass
- [x] No regressions in existing functionality
- [ ] CI pipeline green (Pending remote push)

---

## 6. Debugging Experience

| Problem | Failed Solution | Successful Solution |
|:--------|:---------------|:-------------------|
| Branches have unrelated histories | Direct merge fails | Forward-port develop onto develop-part6 first |
| AnnotationSubtype enum missing in new namespace | Keep using old namespace | Changed to uint8_t (new namespace only has AnnotationType) |
| TypedData::write() uses core::Writer, not util::BitWriter | Pass BitWriter directly | Wrap with core::Writer before calling write() |
| ZSTD codec structure differs | Keep part6's mpeggCodecs-static | Use develop's external find_package(Zstd) pattern |

---

## 7. Codec Structure Decision (Per Step 4)

**PRINCIPLE**: Use develop's codec structure (external packages via `find_package`) rather than part6's internal thirdparty approach.

### ZSTD Decision Matrix

| Aspect | develop | develop-part6 | Merged Result |
|--------|---------|---------------|--------------|
| Build method | `find_package(Zstd)` | `mpeggCodecs-static` | **Use `find_package(Zstd)`** |
| Encoder class | `Encoder : EntropyEncoder` | `ZSTDEncoder` (standalone) | **Adapt to EntropyEncoder base** |
| Decoder class | `Decoder : EntropyDecoder` | Missing | **Restore from develop** |
| Files | decoder.cc, encoder.cc, param_decoder.cc, subsequence.cc | encoder.cc only | **Use develop's full set** |
| Linking | PUBLIC | PRIVATE | **Use PUBLIC** |

### Action: Restore ZSTD to Develop Pattern

```bash
# Before merging entropy, ensure part6's ZSTD has:
# 1. decoder.cc/h (restored from develop)
# 2. param_decoder.cc/h (restored from develop)
# 3. subsequence.cc (renamed from .cpp, restored)
# 4. find_package(Zstd) in CMakeLists.txt
# 5. Encoder : public EntropyEncoder
# 6. Decoder : public EntropyDecoder
```

---

# APPENDIX A: Investigation Methodology

## A.0 What Was Done - Investigation Process

### Step 0: Repository Setup
1. **Cloned fresh develop branch** to `/home/adhisant/workspace/genie-develop`
   ```bash
   git clone -b develop --depth 1 git@github.com:MueFab/genie.git /home/adhisant/workspace/genie-develop
   ```

2. **Verified genie-part6 branch status**
   - Confirmed it tracks origin at git@github.com:MueFab/genie.git
   - Identified branch structure: `develop-part6` diverged from `develop`

### Step 1: Scope Analysis Commands Used

| Command | Purpose |
|---------|---------|
| `git log --oneline develop-part6 ^develop \| head -30` | List commits unique to part6 |
| `git diff develop..develop-part6 --stat` | Summary of changed files |
| `git diff develop..develop-part6 --name-only \| wc -l` | Count changed files |
| `git diff develop..develop-part6 --name-only \| grep -E '^src/genie/' \| wc -l` | Count genie-specific changes |
| `git diff develop..develop-part6 --name-only \| grep -E '^src/genie/[^/]+/' \| sort -u` | List changed modules |
| `git diff develop..develop-part6 --name-status \| grep -E '^(A\|M).*src/genie/(annotation\|backend\|...)/'` | New files per module |

### Step 2: Module Structure Comparison

Used `ls` commands to compare directory structures:
```bash
ls /home/adhisant/workspace/genie-part6/src/genie/
ls /home/adhisant/workspace/genie-develop/src/genie/
```

Identified that part6 has additional modules: `annotation/`, `backend/`, `variantsite/`

### Step 3: CMakeLists Analysis

Compared build system files:
- Root `src/genie/CMakeLists.txt`
- Each module's `CMakeLists.txt`
- Used `diff` command for direct comparison

### Step 4: Thirdparty Comparison

```bash
ls /home/adhisant/workspace/genie-part6/thirdparty/   # Shows bbhash, boost, cli11, codecs, filesystem, kwaymergesort, nlohmann, picosha2
ls /home/adhisant/workspace/genie-develop/thirdparty/ # Shows bbhash, cli11, kwaymergesort, nlohmann, picosha2
```

Identified new thirdparty additions: `filesystem/`, expanded `codecs/` (zstd)

### Step 5: Detailed File-Level Analysis

For entropy, format, and core modules - used targeted diffs:
```bash
git diff develop..develop-part6 -- src/genie/entropy/zstd/encoder.h | head -100
git diff develop..develop-part6 -- src/genie/entropy/zstd/CMakeLists.txt
git diff develop..develop-part6 -- src/genie/format/mgb/CMakeLists.txt
git diff develop..develop-part6 -- src/genie/core/CMakeLists.txt
```

---

# APPENDIX B: Detailed Module Analysis

## B.1 genie-entropy Detailed Comparison

### Modules Present in Both Branches

| Module | Develop | Part6 | Notes |
|--------|---------|-------|-------|
| bsc | ✓ | ✓ | |
| gabac | ✓ | ✓ | |
| jbig | ✓ | ✓ | |
| lzma | ✓ | ✓ | |
| paramcabac | ✓ | ✓ | |
| rans | ✓ | ✓ | |
| **zstd** | ✓ | **MAJOR CHANGES** | API completely rewritten |

### ZSTD File Listing Comparison

**Develop ZSTD files:**
```
CMakeLists.txt
decoder.cc
decoder.h
encoder.cc
encoder.h
param_decoder.cc
param_decoder.h
subsequence.cc
subsequence.h
```

**Part6 ZSTD files:**
```
CMakeLists.txt
decoder.cc
decoder.h
encoder.cc
encoder.h
param_decoder.cc
param_decoder.h
subsequence.cpp    <-- RENAMED from .cc to .cpp
subsequence.h
```

### ZSTD CMakeLists.txt Changes (Full Diff)

**BEFORE (develop):**
```cmake
project("genie-zstd")

set(source_files
        decoder.cc
        encoder.cc
        param_decoder.cc
        subsequence.cc)

if (${GABAC_BUILD_SHARED_LIB})
    add_library(genie-zstd SHARED ${source_files})
else ()
    add_library(genie-zstd STATIC ${source_files})
endif ()

find_package(Zstd REQUIRED)

target_link_libraries(genie-zstd PUBLIC genie-core)
target_link_libraries(genie-zstd PUBLIC ${Zstd_LIBRARIES})

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../../ ABSOLUTE)
target_include_directories(genie-zstd PUBLIC "${THIS_DIR}")
target_include_directories(genie-zstd PUBLIC "${TOP_DIR}")
```

**AFTER (part6):**
```cmake
project("genie-zstd")

set(source_files
    encoder.cc
)
add_library(genie-zstd ${source_files})

target_link_libraries(genie-zstd PRIVATE genie-core)
target_link_libraries(genie-zstd PRIVATE genie-util)
target_link_libraries(genie-zstd PRIVATE mpeggCodecs-static)
```

**Key Observations:**
1. Part6 **REMOVED** `decoder.cc` from source_files
2. Part6 **REMOVED** `find_package(Zstd)` - uses `mpeggCodecs-static` instead
3. Part6 changed from **PUBLIC** to **PRIVATE** linking
4. Part6 added `genie-util` dependency
5. Part6 removed explicit include directory configuration

### ZSTD Encoder API Changes (Complete Comparison)

**Develop encoder.h - Class Definition:**
```cpp
/**
 * @brief Encoder class for the ZSTD compression algorithm.
 * @details This class handles the compression of raw access units into block
 * payloads using the ZSTD algorithm. It converts the raw MPEG-G descriptors
 * into compressed representations, following the MPEG-G specifications and
 * utilizing the GENIE core infrastructure.
 */
class Encoder final : public core::EntropyEncoder {
  bool write_out_streams_{};  //!< @brief Flag to enable writing output streams

  /**
   * @brief Compress a given descriptor using the ZSTD algorithm.
   * @param desc Reference to the descriptor to be compressed.
   * @return The compressed entropy-coded data, encapsulated in the
   * `EntropyCoded` structure.
   */
  entropy_coded Process(core::AccessUnit::Descriptor& desc) override;

  /**
   * @brief Construct a new Encoder object.
   * @param write_out_streams Flag to enable or disable writing out streams for
   * debugging.
   */
  explicit Encoder(bool write_out_streams);
};
```

**Part6 encoder.h - Class Definition:**
```cpp
class ZSTDParameters {
 public:
    bool use_dictionary_flag;
    uint16_t dictionary_size;
    std::string dictionary;
    ZSTDParameters() : use_dictionary_flag(false), dictionary_size(0), dictionary{} {}
    ZSTDParameters(uint8_t _use_dictionary_flag, uint8_t _dictionary_size, std::string _dictionary)
        : use_dictionary_flag(_use_dictionary_flag), dictionary_size(_dictionary_size), dictionary(_dictionary) {}

    genie::core::parameter::annotation::AlgorithmParameters convertToAlgorithmParameters() const;
    genie::core::parameter::annotation::CompressorParameterSet compressorParameterSet(
        uint8_t compressor_ID) const;

    bool parsAreDefault() const { return use_dictionary_flag == false && dictionary_size == 0 && dictionary.empty(); }
};

class ZSTDEncoder {
  public:
    ZSTDEncoder();

    void encode(std::stringstream &input, std::stringstream &output);
    ZSTDParameters getParameters() const;
    void setParameters(const ZSTDParameters& params);
};
```

### ZSTD Encoder Implementation Changes

**Develop encoder.cc - Key Functions:**
```cpp
// Uses ZSTD external library directly
#include <zstd.h>

namespace genie::entropy::zstd {

void StoreParameters(core::GenDesc desc, core::parameter::DescriptorSubSequenceCfg& parameter_set);
core::AccessUnit::Subsequence compress(core::AccessUnit::Subsequence&& in);
core::EntropyEncoder::entropy_coded Encoder::Process(core::AccessUnit::Descriptor& desc);

}  // namespace genie::entropy::zstd
```

**Part6 encoder.cc - Key Functions:**
```cpp
// Uses mpegg internal codec
#include "codecs/api/mpegg_utils.h"
#include "codecs/include/mpegg-codecs.h"

namespace genie::entropy::zstd {

ZSTDEncoder::ZSTDEncoder() : use_dictionary_flag(false), dictionary_size(0), dictionary{} {}

void ZSTDEncoder::encode(std::stringstream &input, std::stringstream &output) {
    const size_t srcLen = input.str().size();
    unsigned char *destination;
    size_t destLen = srcLen;

    int ret = mpegg_zstd_compress(&destination, &destLen,
                                  (const unsigned char *)input.str().c_str(),
                                  srcLen, 0);

    if (ret != 0) {
        std::cerr << "error with zstd compression\n";
    }
    output.write((const char *)destination, destLen);
    if (destination) free(destination);
}

}  // namespace genie::entropy::zstd
```

### ZSTD API Compatibility Assessment

| Aspect | Develop | Part6 | Compatible | Action Required |
|--------|---------|-------|------------|-----------------|
| Class name | `Encoder` | `ZSTDEncoder` | NO | Rename in consuming code |
| Base class | `core::EntropyEncoder` | None | NO | Remove inheritance |
| I/O type | `core::AccessUnit::Descriptor` | `std::stringstream` | NO | Rewrite I/O layer |
| Decoder | Included (`decoder.cc`) | Not in sources | NO | Check if decoder is used |
| External lib | `find_package(Zstd)` | `mpeggCodecs-static` | NO | Change build dependency |
| Linking visibility | PUBLIC | PRIVATE | YES | No code change needed |
| File name | `subsequence.cc` | `subsequence.cpp` | NO | Track rename in git |

### ZSTD Consumer Impact Analysis

**Must check before merge:**
1. Is `genie::entropy::zstd::Encoder` used in:
   - `genie-core`
   - `genie-format`
   - `genie-annotation`
   - Any app-level code
2. Is `genie::entropy::zstd::Decoder` used anywhere?
3. Does `mpeggCodecs-static` provide equivalent ZSTD functionality?

---

## B.2 genie-format Detailed Comparison

### Module Presence Comparison

| Module | Develop | Part6 | Status |
|--------|---------|-------|--------|
| fasta | ✓ | ✓ | **Modified** - file renames |
| fastq | ✓ | ✓ | Unchanged |
| mgb | ✓ | ✓ | **Renamed files** |
| mgg | ✓ | ✓ | **Modified** - new subdirs |
| mgrec | ✓ | ✓ | Unchanged |
| sam | ✓ | **REMOVED** | **DELETED** |

### MGB File Listing Comparison

**Develop MGB files:**
```
CMakeLists.txt
access_unit.cc
access_unit.h
access_unit_header.cc
access_unit_header.h
au_type_cfg.cc
au_type_cfg.h
block.cc
block.h
data_unit_factory.cc      <-- underscore
data_unit_factory.h       <-- underscore
exporter.cc
exporter.h
extended_au.cc
extended_au.h
importer.cc
importer.h
mgb_file.cc
mgb_file.h
mgb_file.impl.h
mm_cfg.cc
mm_cfg.h
raw_reference.cc
raw_reference.h
raw_reference_seq.cc
raw_reference_seq.h
ref_cfg.cc
ref_cfg.h
reference.cc
reference.h
signature_cfg.cc
signature_cfg.h
```

**Part6 MGB files:**
```
CMakeLists.txt
access_unit.cc
access_unit.h
access_unit_header.cc
access_unit_header.h
au_type_cfg.cc
au_type_cfg.h
block.cc
block.h
data-unit-factory.cc      <-- HYPHENATED!
data-unit-factory.h       <-- HYPHENATED!
exporter.cc
exporter.h
extended_au.cc
extended_au.h
importer.cc
importer.h
mgb_file.cc
mgb_file.h
mgb_file.impl.h
mm_cfg.cc
mm_cfg.h
raw_reference.cc
raw_reference.h
raw_reference_seq.cc
raw_reference_seq.h
ref_cfg.cc
ref_cfg.h
reference.cc
reference.h
signature_cfg.cc
signature_cfg.h
```

### MGB CMakeLists.txt Changes

**BEFORE (develop):**
```cmake
set(source_files
        access_unit.cc
        access_unit_header.cc
        block.cc
        raw_reference_seq.cc
        raw_reference.cc

        data_unit_factory.cc

        exporter.cc
        importer.cc

        reference.cc
        mgb_file.cc
        access_unit_header.cc)
```

**AFTER (part6):**
```cmake
set(source_files
        access_unit.cc
        access_unit_header.cc
        block.cc
        raw_reference_seq.cc
        raw_reference.cc

        data-unit-factory.cc   <-- HYPHENATED

        exporter.cc
        importer.cc

        reference.cc
        mgb_file.h mgb_file.cc mgb_file.impl.h access_unit_header.h access_unit_header.cc)
        ^-- NEW: Added .h files and .impl.h that weren't in sources before!
```

### MGG File Listing Comparison

**Develop MGG subdirectories:**
```
access_unit.cc
access_unit.h
access_unit_header.cc
access_unit_header.h
au_information.cc
au_information.h
au_protection.cc
au_protection.h
block.cc
block.h
block_header.cc
block_header.h
box.cc
box.h
data_stream.cc
data_stream.h
dataset.cc
dataset.h
dataset_group.cc
dataset_group.h
dataset_group_header.cc
dataset_group_header.h
dataset_group_metadata.cc
dataset_group_metadata.h
dataset_group_protection.cc
dataset_group_protection.h
dataset_header/
dataset_header.cc
dataset_header.h
dataset_mapping_table.cc
dataset_mapping_table.h
dataset_mapping_table_list.cc
dataset_mapping_table_list.h
dataset_metadata.cc
dataset_metadata.h
dataset_parameter_set/        <-- directory
dataset_parameter_set.cc
dataset_parameter_set.h
dataset_protection.cc
dataset_protection.h
descriptor_stream.cc
descriptor_stream.h
descriptor_stream_header.cc
descriptor_stream_header.h
descriptor_stream_protection.cc
descriptor_stream_protection.h
encapsulator/                 <-- directory
file_header.cc
file_header.h
gen_info.cc
gen_info.h
label.cc
label.h
label_dataset.cc
label_dataset.h
label_list.cc
label_list.h
label_region.cc
label_region.h
master_index_table/           <-- directory
master_index_table.cc
master_index_table.h
mgg_file.cc
mgg_file.h
offset.cc
offset.h
packet.cc
packet.h
packet_header.cc
packet_header.h
reference/
reference.cc
reference.h
reference_metadata.cc
reference_metadata.h
```

**Part6 MGG subdirectories (ADDITIONS):**
```
annotation_access_unit/       <-- NEW
annotation_table/             <-- NEW
au_information.cc
... (same base files) ...
dataset_header/               <-- NEW (was file in develop)
dataset_parameterset/         <-- NEW (was dataset_parameter_set in develop)
encapsulator/                 <-- exists in develop too
master_index_table/           <-- exists in develop too
reference/                    <-- exists in develop too
```

### MGG New Subdirectory Details

**Part6 has these NEW or EXPANDED subdirectories:**

| Subdirectory | Contents | Status |
|--------------|----------|--------|
| `annotation_access_unit/` | annotation access unit handling | NEW |
| `annotation_table/` | annotation table handling | NEW |
| `dataset_header/` | dataset header files | NEW (was flat files in develop) |
| `dataset_parameterset/` | dataset parameter set | NEW (develop had `dataset_parameter_set/`) |
| `encapsulator/` | file encapsulator | EXISTS in both |
| `master_index_table/` | MIT handling | EXISTS in both |
| `reference/` | reference handling | EXISTS in both |

### FASTA File Changes

**Develop FASTA files:**
```
CMakeLists.txt
exporter.cc
fai_file.cc
fasta_source.cc
manager.cc
reader.cc
reference.cc
sha256File.cc
../sam/sam_sorter.h     <-- external dependency
```

**Part6 FASTA files:**
```
CMakeLists.txt
exporter.cc
fai-file.cc              <-- HYPHENATED
fasta-source.cc          <-- HYPHENATED
manager.cc
reader.cc
reference.cc
sha256File.cc
(sam_sorter.h dependency removed)
```

### Format Module Merge Recommendations

| Module | Issue | Resolution |
|--------|-------|------------|
| **MGB** | File renamed `data_unit_factory.cc` → `data-unit-factory.cc` | Use `git mv data_unit_factory.cc data-unit-factory.cc` to preserve history |
| **MGB** | New files in CMakeLists: `mgb_file.h`, `mgb_file.impl.h`, `access_unit_header.h` | Add these files to merged CMakeLists |
| **MGG** | New subdirs: `annotation_access_unit/`, `annotation_table/`, `dataset_header/`, `dataset_parameterset/` | Accept all new subdirectories |
| **FASTA** | Files renamed: `fai_file.cc` → `fai-file.cc`, `fasta_source.cc` → `fasta-source.cc` | Use `git mv` to preserve history |
| **FASTA** | Removed `../sam/sam_sorter.h` dependency | Confirm SAM removal doesn't break FASTA |
| **SAM** | All SAM files removed from part6 | VERIFY if intentional - may break existing functionality |

---

## B.3 genie-core Detailed Comparison

### CMakeLists.txt Structural Differences

**Develop CMakeLists.txt approach:**
- Flat list of source files
- Direct `add_library(genie-core ${source_files})`
- Simple include directory setup
- Minimal compiler flags

**Part6 CMakeLists.txt approach:**
- Hierarchical organization with subdirectories
- Uses `${PROJECT_NAME}` variable instead of hardcoded name
- Converts relative paths to absolute paths using loops
- Adds extensive compiler flags for GCC/Clang
- New dependency on `genie-backend`

### Part6 Source File Additions (New Subdirectories)

**1. Variant Record Handling (NEW):**
```cmake
record/variant/record.cc
record/variant/format_field.cc
```

**2. Linked Record (NEW):**
```cmake
linked_record/linked_record.cc
```

**3. Site and Data Unit Records (NEW):**
```cmake
record/site/record.cc
record/data_unit/record.cc
```

**4. Access Unit Annotation Subdirectory (NEW - 7 files):**
```cmake
access_unit/annotation/record.cc
access_unit/annotation/block.cc
access_unit/annotation/block_header.cc
access_unit/annotation/block_payload.cc
access_unit/annotation/block_payload_stream.cc
access_unit/annotation/annotation_access_unit_header.cc
access_unit/annotation/typed_data.cc
```

**5. Parameter Annotation Subdirectory (NEW - 8 files):**
```cmake
parameter/annotation/record.cc
parameter/annotation/attribute_parameter_set.cc
parameter/annotation/algorithm_parameters.cc
parameter/annotation/tile_configuration.cc
parameter/annotation/tile_structure.cc
parameter/annotation/descriptor_configuration.cc
parameter/annotation/compressor_parameter_set.cc
parameter/annotation/annotation_encoding_parameters.cc
parameter/annotation/attribute_data.cc
```

**6. New Utility Files:**
```cmake
array_type.cc
writer.cc
```

**7. Contact Record (NEW):**
```cmake
record/contact/record.cc
```

**8. Renamed Access Unit:**
```cmake
access_unit.cc  →  access_unit/access_unit.cc
```

### CMakeLists.txt Build System Changes (Complete Diff)

**BEFORE (develop):**
```cmake
project("genie-core")

set(source_files
        parameter/descriptor_present/decoder.cc
        parameter/descriptor_present/decoder_regular.cc
        # ... (flat list continues)
        api.cc
        c_api.cc
        payload.cc)

add_library(genie-core ${source_files})

target_link_libraries(genie-core PUBLIC genie-util)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(genie-core PUBLIC "${TOP_DIR}")
```

**AFTER (part6):**
```cmake
project("genie-core")

set(source_files
        # ... same base files ...
        # PLUS new additions:
        record/variant/record.cc
        record/variant/format_field.cc
        linked_record/linked_record.cc
        record/site/record.cc
        record/data_unit/record.cc
        access_unit/annotation/record.cc
        access_unit/annotation/block.cc
        # ... (all annotation files)
        parameter/annotation/record.cc
        parameter/annotation/attribute_parameter_set.cc
        # ... (all annotation parameter files)
        array_type.cc
        writer.cc
        record/contact/record.cc
        access_unit/access_unit.cc  # RENAMED
        api.cc
        c_api.cc
        payload.cc
)

# Convert relative paths to absolute
set(absolute_source_files "")
foreach(f IN LISTS source_files)
    if(IS_ABSOLUTE "${f}")
        list(APPEND absolute_source_files "${f}")
    else()
        list(APPEND absolute_source_files "${CMAKE_CURRENT_LIST_DIR}/${f}")
    endif()
endforeach()

add_library(${PROJECT_NAME} ${absolute_source_files})

# NEW: Compiler flags
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE "-pedantic" "-fno-common" "-Wall" "-Wshadow" "-Wextra" "-Wundef" "-Wconversion" "-Wdouble-promotion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=conversion" "-Wno-error=maybe-uninitialized")
endif ()

# CHANGED: Added genie-backend dependency
target_link_libraries(${PROJECT_NAME} PUBLIC genie-util genie-backend)

get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(${PROJECT_NAME} PUBLIC "${TOP_DIR}")
```

### Core New Subdirectory Summary

| Subdirectory | Files | Purpose | Lines of Code (Est.) |
|--------------|-------|---------|---------------------|
| `access_unit/annotation/` | 7 | Annotation access unit handling | ~2000 |
| `parameter/annotation/` | 8 | Annotation-specific parameters | ~1500 |
| `linked_record/` | 1 | Linked record implementation | ~300 |
| `record/variant/` | 2 | Variant genomic records | ~500 |
| `record/site/` | 1 | Site records | ~200 |
| `record/data_unit/` | 1 | Data unit records | ~200 |
| `record/contact/` | 1 | Contact records | ~200 |

### Core New Files Analysis

| File | Description | Dependency |
|------|-------------|------------|
| `array_type.cc/h` | Array type utilities for backend abstraction | None |
| `ndarray.cc/h` | N-dimensional array wrapper (referenced but may be pre-existing) | Backend |
| `writer.cc/h` | Writer bypass architecture | None |

### Core Dependency Changes

**Critical change - genie-core now depends on genie-backend:**
```diff
- target_link_libraries(genie-core PUBLIC genie-util)
+ target_link_libraries(${PROJECT_NAME} PUBLIC genie-util genie-backend)
```

**Impact**: This creates a circular dependency risk if not carefully managed:
- `genie-backend` depends on `genie-util`
- `genie-core` depends on `genie-backend`
- But annotation may need both

### Core Merge Strategy (CRITICAL - Step by Step)

**Phase 1: Preparation**
1. Start with develop CMakeLists.txt as base
2. Document all original source files

**Phase 2: Additions (in order of complexity)**
1. Add `linked_record/linked_record.cc` (1 file, minimal impact)
2. Add `record/variant/` files (2 files)
3. Add `record/site/record.cc` and `record/data_unit/record.cc`
4. Add `record/contact/record.cc`
5. Add `array_type.cc` and `writer.cc`
6. Add `parameter/annotation/` files (8 files)
7. Add `access_unit/annotation/` files (7 files)

**Phase 3: Rename**
- Rename `access_unit.cc` → `access_unit/access_unit.cc`

**Phase 4: Build System**
- Add genie-backend dependency
- Add compiler flags

**Phase 5: Verification**
- Build after each phase
- Test after each phase

---

## B.4 Thirdparty Additions Analysis

### Thirdparty Comparison

**Develop thirdparty:**
```
bbhash/
cli11/
kwaymergesort/
nlohmann/
picosha2/
```

**Part6 thirdparty:**
```
bbhash/
boost/
cli11/
codecs/
filesystem/
kwaymergesort/
nlohmann/
picosha2/
```

### New Thirdparty Components

| Component | Path | Purpose | Size (Est.) |
|-----------|------|---------|-------------|
| ZSTD codec | `thirdparty/codecs/zstd/` | Compression | ~50 files |
| Boost | `thirdparty/boost/` | Utilities | ~100 files |
| Filesystem | `thirdparty/filesystem/` | Filesystem header-only lib | ~10 files |
| Nlohmann JSON (updated) | `thirdparty/nlohmann/` | JSON parsing | existing, may be updated |

### Codecs Directory Comparison

**Develop codecs:**
```
# (may contain lzma, bsc, gabac internal headers)
```

**Part6 codecs:**
```
jbig/
lzma/
zstd/    <-- NEW ZSTD codec
```

---

## B.5 API Compatibility Summary Matrix

| Module/Component | Develop API | Part6 API | Compatible | Migration Effort |
|------------------|-------------|-----------|------------|------------------|
| ZSTD Encoder class | `Encoder : EntropyEncoder` | `ZSTDEncoder` | **NO** | HIGH - full rewrite |
| ZSTD Encoder I/O | `AccessUnit::Descriptor` | `std::stringstream` | **NO** | HIGH - full rewrite |
| ZSTD Decoder | Present | **Removed** from sources | **NO** | Check usage |
| ZSTD External lib | `find_package(Zstd)` | `mpeggCodecs-static` | **NO** | Build config change |
| MGB Factory | `data_unit_factory` | `data-unit-factory` | **RENAME** | LOW - git mv |
| MGG structure | `dataset_parameter_set/` | `dataset_parameterset/` + new subdirs | **NO** | MEDIUM |
| FASTA files | `fai_file.cc` | `fai-file.cc` | **RENAME** | LOW - git mv |
| Core AccessUnit | `access_unit.cc` (flat) | `access_unit/access_unit.cc` (hierarchical) | **NO** | MEDIUM - path updates |
| Core Backend dep | None | `genie-backend` | **NEW** | LOW - add dependency |
| SAM module | Present | **REMOVED** | **NO** | UNKNOWN - verify intent |

---

## B.6 Risk Assessment Detail

### HIGH Risk Areas

**1. ZSTD API Complete Rewrite**
- **Risk**: Breaking change to entropy encoding
- **Impact**: Any code using `genie::entropy::zstd::Encoder` will break
- **Verification required**: Find all usages of ZSTD encoder before merge

**2. genie-core ↔ genie-backend Dependency**
- **Risk**: Circular dependency or build order issues
- **Impact**: Core now requires backend, but core was foundational
- **Verification required**: Ensure build order is correct

**3. SAM Module Removal**
- **Risk**: Unknown if intentional or accidental
- **Impact**: Any SAM-related functionality will break
- **Verification required**: Confirm with stakeholder if SAM removal is intentional

### MEDIUM Risk Areas

**4. MGB File Renames**
- **Risk**: Git history fragmentation
- **Impact**: `data_unit_factory` → `data-unit-factory` loses history if not properly renamed
- **Mitigation**: Use `git mv` to preserve history

**5. MGG New Subdirectories**
- **Risk**: Build system may not include new directories
- **Impact**: Some format handling code may be missing
- **Mitigation**: Verify CMakeLists includes all new subdirectories

**6. Core Compiler Flags**
- **Risk**: Different flags than rest of project
- **Impact**: Inconsistent warnings/errors across modules
- **Mitigation**: Consider aligning with project's existing flags

### LOW Risk Areas

**7. FASTA File Renames**
- **Risk**: Minor path changes
- **Impact**: Include path updates required
- **Mitigation**: Use `git mv` for history preservation

**8. genie-util Deletions**
- **Risk**: Removing unused code
- **Impact**: No impact if truly unused
- **Mitigation**: Verify no dependencies before removal

---

## B.7 Pre-Merge Verification Checklist

Before executing merge, verify:

- [ ] **ZSTD Usage Audit**: Find all code using `genie::entropy::zstd::Encoder`
  ```bash
  grep -r "entropy::zstd::Encoder" --include="*.cc" --include="*.h" /home/adhisant/workspace/genie-part6/src/
  ```

- [ ] **ZSTD Decoder Usage Audit**: Check if decoder is used anywhere
  ```bash
  grep -r "zstd.*Decoder" --include="*.cc" --include="*.h" /home/adhisant/workspace/genie-part6/src/
  ```

- [ ] **SAM Dependency Check**: Verify FASTA doesn't depend on SAM
  ```bash
  grep -r "sam_sorter" --include="*.cc" --include="*.h" /home/adhisant/workspace/genie-part6/src/genie/format/
  ```

- [ ] **Backend Usage Audit**: Find all code using backend
  ```bash
  grep -r "genie::backend" --include="*.cc" --include="*.h" /home/adhisant/workspace/genie-part6/src/
  ```

- [ ] **Core Dependency Check**: Verify core → backend dependency is acceptable
  ```bash
  grep -r "genie-core.*genie-backend" --include="*.cmake" --include="CMakeLists.txt" /home/adhisant/workspace/genie-part6/src/
  ```

---

## B.8 Post-Merge Testing Requirements

### Unit Test Requirements

| Module | Test Binary | Key Tests |
|--------|-------------|-----------|
| util | `genie-test-util` | Core utilities |
| core | `genie-test-core` | Access unit, records |
| backend | `genie-test-backend` | Backend dispatch |
| entropy | `genie-test-entropy` | ZSTD, LZMA, BSC encode/decode |
| format | `genie-test-format` | MGB, MGG read/write |
| contact | `genie-test-contact` | Golden Master |
| genotype | `genie-test-genotype` | Golden Master |
| likelihood | `genie-test-likelihood` | Golden Master |
| annotation | `genie-test-annotation` | If exists |

### Build Matrix Verification

All 9 combinations must pass:
- Contact × {STD, Eigen, XTensor}
- Genotype × {STD, Eigen, XTensor}
- Likelihood × {STD, Eigen, XTensor}

---

# APPENDIX C: Complete Git Commands for Execution

## C.1 Initial Setup

```bash
# Backup develop
cd /home/adhisant/workspace/genie-develop
git bundle create /tmp/develop-backup.bundle develop

# Backup part6
cd /home/adhisant/workspace/genie-part6
git bundle create /tmp/part6-backup.bundle develop-part6

# Verify clean state
git status
git log --oneline -1  # Should be latest commit
```

## C.2 Step-by-Step Merge Commands

### Step 1: Merge genie-util
```bash
cd /home/adhisant/workspace/genie-develop
git fetch origin
git checkout develop
git merge origin/develop-part6 --no-commit
# Resolve conflicts in src/genie/util/
git add src/genie/util/
git commit -m "merge(genie-util): part6 changes"
```

### Step 2: Merge genie-module
```bash
git merge origin/develop-part6 --no-commit
# Resolve conflicts in src/genie/module/
git add src/genie/module/
git commit -m "merge(genie-module): part6 changes"
```

### Step 3: Add genie-backend (NEW)
```bash
git add src/genie/backend/
git commit -m "feat(backend): add new backend abstraction layer"
```

### Step 4: Merge genie-entropy
```bash
# Track file rename for subsequence.cc → .cpp
git mv src/genie/entropy/zstd/subsequence.cc src/genie/entropy/zstd/subsequence.cpp || true
git add src/genie/entropy/
git commit -m "merge(genie-entropy): zstd, bsc, lzma, jbig"
```

### Step 5: Merge genie-format
```bash
# Track file renames for MGB and FASTA
git mv src/genie/format/mgb/data_unit_factory.cc src/genie/format/mgb/data-unit-factory.cc || true
git mv src/genie/format/mgb/data_unit_factory.h src/genie/format/mgb/data-unit-factory.h || true
git mv src/genie/format/fasta/fai_file.cc src/genie/format/fasta/fai-file.cc || true
git mv src/genie/format/fasta/fasta_source.cc src/genie/format/fasta/fasta-source.cc || true
git add src/genie/format/
git commit -m "merge(genie-format): mgb, mgg, fasta"
```

### Step 6: Merge genie-core (CRITICAL)
```bash
# This step requires careful CMakeLists.txt merging
# Suggest doing this in a separate branch or with full backup
git add src/genie/core/
git commit -m "merge(genie-core): full restructure with annotation support"
```

### Steps 7-9: Merge coders
```bash
git add src/genie/contact/
git commit -m "refactor(contact): unified backend dispatcher"

git add src/genie/genotype/
git commit -m "refactor(genotype): unified backend dispatcher"

git add src/genie/likelihood/
git commit -m "refactor(likelihood): unified backend dispatcher"
```

### Step 10: Add genie-annotation (NEW)
```bash
git add src/genie/annotation/
git add src/genie/CMakeLists.txt
git commit -m "feat(annotation): add new annotation module"
```

### Step 11: Finalize root CMakeLists
```bash
git add CMakeLists.txt
git commit -m "merge: update build system for part6 modules"
```

### Step 12: Verify
```bash
mkdir build && cd build
cmake -DBUILD_TESTS=ON -DGENIE_BACKEND=std ..
make -j8
ctest --output-on-failure
```

---

## B.9 Quick Reference - File Renames Required

| Original (develop) | Target (part6) | Module |
|-------------------|----------------|--------|
| `data_unit_factory.cc/h` | `data-unit-factory.cc/h` | MGB |
| `fai_file.cc` | `fai-file.cc` | FASTA |
| `fasta_source.cc` | `fasta-source.cc` | FASTA |
| `subsequence.cc` | `subsequence.cpp` | ZSTD |
| `access_unit.cc` | `access_unit/access_unit.cc` | Core |

---

---

# APPENDIX D: develop-part6 Changes Since Merge Base

## D.1 Summary

Since the merge base `934b5ca9` (shared history creation), the following files were modified in `develop-part6`:

```
src/genie/annotation/accessunit_composer.cc
src/genie/annotation/attributes.h
src/genie/annotation/geno_annotation.cc
src/genie/annotation/tiles.h
src/genie/annotation/variantsite_parser.h
src/genie/annotation/vsite_attributes.cc
src/genie/annotation/vsite_tiles.cc
src/genie/core/access_unit/annotation/typed_data.cc
src/genie/core/access_unit/annotation/typed_data.h
src/genie/variantsite/accessunit_composer.cc
src/genie/variantsite/attributes.h
```

**Total: 11 files changed, 55 insertions(+), 33 deletions(-)**

---

## D.2 Detailed Changes by File

### 1. `core/access_unit/annotation/typed_data.{cc,h}` (NEW METHOD)

**Change**: Added `write(util::BitWriter&)` method alongside existing `write(core::Writer&)`.

```cpp
// typed_data.h - Added new method declaration
void write(util::BitWriter& writer) const;

// typed_data.cc - Added new method implementation
void TypedData::write(util::BitWriter& writer) const {
    writer.WriteBits(static_cast<uint8_t>(data_type_ID), 8);
    writer.WriteBits(num_array_dims, 2);
    uint64_t n_elements = 1;
    for (uint64_t idx_i = 0; idx_i < num_array_dims; ++idx_i) {
        writer.WriteBits(array_dims[idx_i], 32);
        n_elements = n_elements * array_dims[idx_i];
    }

    if (!compressedDataStream.str().empty()) {
        bool encoded = true;
        writer.WriteBits(encoded, 1);
        auto size = compressedDataStream.str().size();
        writer.WriteBits(size, 32);
        writer.Write(const_cast<std::stringstream*>(&compressedDataStream));
    } else {
        bool encoded = false;
        writer.WriteBits(encoded, 1);
        writer.Write(const_cast<std::stringstream*>(&dataStream));
    }
    writer.FlushBits();
}
```

---

### 2. `annotation/accessunit_composer.cc`

**Change**: Replaced `core::Writer` with `util::BitWriter`.

```diff
-        genie::core::Writer writer(&data);
+        genie::util::BitWriter writer(data);
         tile.second.write(writer);
-        writer.Flush();
+        writer.FlushBits();
```

---

### 3. `annotation/attributes.h`

**Changes**:
- Replaced `#include "genie/core/writer.h"` with `#include "genie/util/bit_writer.h"`
- Changed `writers.back().Flush()` to `writers.back().FlushBits()`
- Changed `writers.back().GetBitsWritten()` to `writers.back().GetTotalBitsWritten()`
- Changed `std::vector<genie::core::Writer>` to `std::vector<genie::util::BitWriter>`

```diff
-#include "genie/core/writer.h"
+#include "genie/util/bit_writer.h"

-  writers.back().Flush();
+  writers.back().FlushBits();

-  uint64_t getCurrentsize() const {return writers.back().GetBitsWritten(); }
+  uint64_t getCurrentsize() const {return writers.back().GetTotalBitsWritten(); }

-    std::vector<genie::core::Writer> writers;
+    std::vector<genie::util::BitWriter> writers;
```

---

### 4. `annotation/geno_annotation.cc`

**Change**: Replaced `core::Writer` with `util::BitWriter`.

```diff
-      genie::core::Writer writer(
-          &descriptorStream[genie::core::AnnotDesc::LIKELIHOOD]);
+      genie::util::BitWriter writer(descriptorStream[genie::core::AnnotDesc::LIKELIHOOD]);
       combined.blocks.at(blockIndex).likelihoodPayload.write(writer);
```

---

### 5. `annotation/tiles.h`

**Changes**:
- Replaced `#include "genie/core/writer.h"` with `#include "genie/util/bit_writer.h"`
- Changed `tileWriter.emplace_back(&tileData.back())` to `tileWriter.emplace_back(tileData.back())`
- Changed `std::vector<genie::core::Writer>` to `std::vector<genie::util::BitWriter>`
- Changed `Write(value, bits)` to `WriteBits(value, bits)`
- Changed `Flush()` to `FlushBits()`
- Changed `GetBitsWritten()` to `GetTotalBitsWritten()`

```diff
-    std::vector<genie::core::Writer> tileWriter;
+    std::vector<genie::util::BitWriter> tileWriter;

-        tiles.tileWriter.back().Write(value, bits);
+        tiles.tileWriter.back().WriteBits(value, bits);

-    void wrapUp() { tiles.tileWriter.back().Flush(); }
+    void wrapUp() { tiles.tileWriter.back().FlushBits(); }

-    size_t getBitsWrittenInTile(size_t tilenr) { return tiles.tileWriter.at(tilenr).GetBitsWritten(); }
+    size_t getBitsWrittenInTile(size_t tilenr) { return tiles.tileWriter.at(tilenr).GetTotalBitsWritten(); }
```

---

### 6. `annotation/variantsite_parser.h`

**Change**: Replaced `core::Writer` with `util::BitWriter`.

```diff
-#include "genie/core/writer.h"
+#include "genie/util/bit_writer.h"

-    std::vector<genie::core::Writer> fieldWriter;
+    std::vector<genie::util::BitWriter> fieldWriter;

-    std::map<std::string, genie::core::Writer> attrWriter;
+    std::map<std::string, genie::util::BitWriter> attrWriter;
```

---

### 7. `annotation/vsite_attributes.cc`

**Changes**:
- Changed `writers.back().Flush()` to `writers.back().FlushBits()`
- Changed `writers.emplace_back(&tiles.back())` to `writers.emplace_back(tiles.back())`
- Changed `core::Writer writer(&TypedTiles.back())` to `genie::util::BitWriter writer(TypedTiles.back())`

```diff
-        writers.back().Flush();
+        writers.back().FlushBits();

-        writers.emplace_back(&tiles.back());
+        writers.emplace_back(tiles.back());

-        core::Writer writer(&TypedTiles.back());
+        genie::util::BitWriter writer(TypedTiles.back());
```

---

### 8. `annotation/vsite_tiles.cc`

**Changes**:
- Changed `Write(value)` to `WriteBits(value, 8)` for string termination
- Changed `tiles.tileWriter.back().Flush()` to `tiles.tileWriter.back().FlushBits()`
- Changed `tiles.tileWriter.emplace_back(&tiles.tileData.back())` to `tiles.tileWriter.emplace_back(tiles.tileData.back())`

```diff
-    tiles.tileWriter.back().Write(0, 8);
+    tiles.tileWriter.back().WriteBits(0, 8);

-      tiles.tileWriter.back().Flush();
+      tiles.tileWriter.back().FlushBits();

-        tiles.tileWriter.emplace_back(&tiles.tileData.back());
+        tiles.tileWriter.emplace_back(tiles.tileData.back());
```

---

### 9. `variantsite/accessunit_composer.cc`

**Change**: Replaced `core::Writer` with direct `util::BitWriter` usage.

```diff
-        util::BitWriter writer(&data);
+        util::BitWriter writer(data);
         for (auto& oneBlock : tileData.second) {
-          core::Writer coreWriter(&data);
-          oneBlock.write(coreWriter);
+          oneBlock.write(writer);
         }
```

---

### 10. `variantsite/attributes.h`

**Same changes as `annotation/attributes.h`**:
- Replaced `#include "genie/core/writer.h"` with `#include "genie/util/bit_writer.h"`
- Changed `Flush()` to `FlushBits()`
- Changed `GetBitsWritten()` to `GetTotalBitsWritten()`
- Changed `std::vector<genie::core::Writer>` to `std::vector<genie::util::BitWriter>`

---

## D.3 `array_type.{cc,h}` Status

**IMPORTANT**: `array_type.{cc,h}` is a **part6-only file** - it does NOT exist in the `develop` branch.

```bash
$ git show develop:src/genie/core/array_type.h
fatal: path 'src/genie/core/array_type.h' exists on disk, but not in 'develop'
```

### Current State in develop-part6

The file is **unchanged since the merge base** (`934b5ca9`). It provides:
- `toArray()` methods for converting BitReader/values to byte arrays
- `toFile()` methods for writing to core::Writer or util::BitWriter
- Data type conversion utilities

### Method Signatures (from array_type.h)

```cpp
class ArrayType {
  private:
    uint8_t bitSize = 0;

  public:
    std::vector<uint8_t> toArray(DataType type, util::BitReader& reader) const;

    void toFile(core::DataType type, std::vector<uint8_t> bytearray, core::Writer& writer) const;
    void toFile(core::DataType type, std::vector<uint8_t> bytearray, util::BitWriter& writer) const;
    void toFile(core::DataType type, util::BitReader& reader, core::Writer& writer, uint64_t number) const;
    void toFile(core::DataType type, util::BitReader& reader, util::BitWriter& writer, uint64_t number) const;
    std::string toString(core::DataType type, std::vector<uint8_t> value) const;

    uint8_t getDefaultBitsize(core::DataType type) const;
    uint64_t getDefaultValue(core::DataType type) const;
    std::vector<uint8_t> toArray(core::DataType type, uint64_t value) const;
};
```

### Merge Recommendation for array_type

Since this file doesn't exist in develop, during merge it should be treated as a **NEW file addition** (like genie-backend). It provides dual-write capability for both `core::Writer` and `util::BitWriter`.

---

## D.4 Pattern Analysis: core::Writer → util::BitWriter Migration

All modified files follow the same migration pattern:

| Original | Migrated To | Notes |
|----------|-------------|-------|
| `core::Writer` | `util::BitWriter` | Direct replacement |
| `Writer(&stream)` | `BitWriter(stream)` | Constructor change (pointer → reference) |
| `writer.Flush()` | `writer.FlushBits()` | Method rename |
| `writer.GetBitsWritten()` | `writer.GetTotalBitsWritten()` | Method rename |
| `writer.Write(value, bits)` | `writer.WriteBits(value, bits)` | Method rename |
| `writer.Write(string)` | `writer.Write(string)` | Same method name, different signature |

---

## D.5 Build & Test Status

**Build**: Clean compile with no errors
**Tests**: All 149 tests pass across 13 test binaries

| Test Binary | Tests | Status |
|-------------|-------|--------|
| genie-test-annotation | 18 | ✓ |
| genie-test-annotation-parameter-set | 29 | ✓ |
| genie-test-conformance | 12 | ✓ |
| genie-test-contact | 21 | ✓ |
| genie-test-core | 18 | ✓ |
| genie-test-entropy | 10 | ✓ |
| genie-test-example | 1 | ✓ |
| genie-test-genotype | 17 | ✓ |
| genie-test-likelihood | 6 | ✓ |
| genie-test-records | 13 | ✓ |
| genie-test-variant-genotype-record | 3 | ✓ |
| genie-test-variant-site-record | 1 | ✓ |

---

**Awaiting confirmation.**