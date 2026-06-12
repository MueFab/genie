# Analysis of Codec Interfaces in GENIE

This document details the interface and implementation of the Genomic Adaptive Binary Arithmetic Coding (GABAC) codec in GENIE, evaluates its structure against other codecs (ZSTD, LZMA, BSC), and discusses the architectural applicability of these designs to specialized codecs like JBIG.

---

## 1. GABAC Encoder/Decoder Interface

GABAC implements the standard MPEG-G (ISO/IEC 23092-2) entropy coding pipeline. In GENIE, GABAC wraps sequence-specific transformations and binary arithmetic coding under the flow graph's unified interface.

### 1.1 GABAC Encoder (`src/genie/entropy/gabac/encoder.h`)
- **Base Class:** `class Encoder : public core::EntropyEncoder`
- **Primary Interface:**
  ```cpp
  entropy_coded Process(core::AccessUnit::Descriptor& desc) override;
  ```
- **Internal Mechanisms:**
  - **Access Unit Partitioning:** GABAC takes a genomic descriptor containing one or more uncompressed subsequences.
  - **Subsequence Transformations:** Symbols are transformed using sequence-specific configurations (e.g., Equality, Run-Length, Match, or Mismatch transforms) to minimize entropy.
  - **Arithmetic Coding:** The transformed binary symbols are processed by Context-Adaptive Binary Arithmetic Coding (CABAC) using dynamic probability state tables.
  - **Output:** Returns an `entropy_coded` object, which encapsulates the serialized byte stream.

### 1.2 GABAC Decoder (`src/genie/entropy/gabac/decoder.h`)
- **Base Class:** `class Decoder : public core::EntropyDecoder`
- **Primary Interface:**
  ```cpp
  std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> Process(
      const core::parameter::DescriptorSubSequenceCfg& param,
      core::AccessUnit::Descriptor& d, bool mm_coder_enabled) override;
  ```
- **Internal Mechanisms:**
  - **CABAC Decompression:** Reads context states and decodes bits from the arithmetic coding stream.
  - **Inverse Transformations:** Applies inverse transformations to restore the original integer or character values.
  - **Output:** Populates the `AccessUnit::Descriptor` with raw subsequences and records performance metrics (e.g., decompression time).

---

## 2. General-Purpose Codecs (ZSTD, LZMA, BSC)

Other general-purpose codecs follow a similar dual-layer structure:
1. **Low-level Engine:** A lightweight wrapper (e.g., `zstd::ZSTDEncoder`, `bsc::BSCEncoder`) wrapping system/third-party API calls (`ZSTD_compress`, `lzma_code`, `libbsc`) with simple stream-based APIs (`encode(stringstream&, stringstream&)`).
2. **Integration Layer:** An adapter subclassing `core::EntropyEncoder` and `core::EntropyDecoder` that routes the flow graph descriptor data through the engine wrapper.

These codecs are registered dynamically inside `src/genie/module/default_setup.cc` and selected dynamically based on sequence dataset parameters.

---

## 3. Applicability to JBIG

While GABAC, ZSTD, LZMA, and BSC process linear 1D arrays of genomic descriptors, JBIG (Joint Bi-level Image Experts Group) is designed specifically for bi-level (binary) 2D matrices.

### 3.1 Why the Unified Flow-Graph Interface Does NOT Apply to JBIG:
- **Dimension Requirements:** JBIG Kit compression requires the explicit width (`ncols`) and height (`nrows`) of the image, as well as alignment configurations. The `core::EntropyEncoder::Process` API takes only a linear `AccessUnit::Descriptor` list and lacks a standard channel for 2D matrix shape parameters.
- **Specific Use Case:** JBIG is not used in GENIE to encode linear genomic descriptors (like read names, positions, qualities). Instead, it is used exclusively to compress binary 2D matrices in specialized coder modules:
  - **Contact Coder:** For subcontact matrix (SCM) tiles.
  - **Genotype Coder:** For genotype/phasing matrices.
- **Direct Execution:** Because JBIG operations require dimensions, the contact and genotype coder modules bypass the flow graph and invoke the low-level `jbig::JBIGEncoder` and `jbig::JBIGDecoder` wrappers directly.

### 3.2 JBIG API Design Reference
Following our refactoring, the JBIG APIs reside under `src/genie/entropy/jbig/` with clean interface segregation:

- **`jbig::JBIGEncoder` (`encoder.h`):**
  ```cpp
  void configure(const JBIGparameters jbigParameters);
  void encode(std::stringstream& input, std::stringstream& output, uint32_t ncols, uint32_t nrows);
  void encode(std::vector<uint8_t>& input, std::vector<uint8_t>& output, uint32_t ncols, uint32_t nrows);
  ```

- **`jbig::JBIGDecoder` (`decoder.h`):**
  ```cpp
  void decode(std::stringstream& input, std::stringstream& output, uint32_t& ncols, uint32_t& nrows);
  void decode(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, uint32_t& ncols, uint32_t& nrows);
  ```

This decoupled model ensures optimal performance and prevents dimension-signaling overhead in the general-purpose entropy coding pipeline.
