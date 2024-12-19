/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder_stub.h
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for the basecoder `EncoderStub` class.
 *
 * This file defines the `EncoderStub` class, which serves as a base class for
 * aligned record encoders in the GENIE framework. It handles the encoding of
 * record sequences, quality values, and read names for a chunk of genomic data.
 *
 * @details The `EncoderStub` class provides a common interface and set of
 * utilities for encoding genomic data records. It interacts with the core GENIE
 * components to produce a structured and compressed output format.
 */

#ifndef SRC_GENIE_READ_BASECODER_ENCODER_STUB_H_
#define SRC_GENIE_READ_BASECODER_ENCODER_STUB_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>
#include <utility>

#include "genie/core/read_encoder.h"
#include "genie/read/basecoder/encoder.h"

// -----------------------------------------------------------------------------

namespace genie::read::basecoder {

/**
 * @brief Common stub for all aligned record encoders.
 *
 * This class provides a base implementation for the core operations of encoding
 * genomic records. It manages state for the encoding process and defines the
 * interface for specialized record encoders.
 */
class EncoderStub : public core::ReadEncoder {
 public:
  /**
   * @brief Structure for managing the encoding state of a chunk.
   */
  struct EncodingState {
    /**
     * @brief Initializes the state from a chunk of records.
     * @param data Chunk of records.
     */
    explicit EncodingState(const core::record::Chunk& data);

    /// Object handling read encoding.
    Encoder read_coder;

    /// Indicates if records are paired-end.
    bool paired_end;

    /// Length of the read being encoded.
    size_t read_length;

    /// Class type of the record.
    core::record::ClassType class_type;

    /// Minimum position in the chunk.
    uint64_t min_pos;

    /// Maximum position in the chunk.
    uint64_t max_pos;

    /// Reference sequence ID.
    uint16_t ref;

    /// Number of segments in the chunk.
    uint64_t read_num;

    /// Last processed read position.
    uint64_t last_read_position;

    /**
     * @brief Virtual destructor for the state structure.
     */
    virtual ~EncodingState() = default;
  };

  /**
   * @brief Get the reference sequences used to encode a record.
   * @param r Record to process.
   * @param state Current state of the encoder.
   * @return Pair of reference sequences. If unpaired, the second reference
   * will be empty.
   */
  virtual std::pair<std::string, std::string> GetReferences(
      const core::record::Record& r, EncodingState& state) = 0;

  /**
   * @brief Create a specialized state for the encoder.
   * @param data Chunk of records to initialize the state from.
   * @return Allocated state object.
   */
  [[nodiscard]] virtual std::unique_ptr<EncodingState> CreateState(
      const core::record::Chunk& data) const = 0;

  /**
   * @brief Encode the base sequences of all records in a chunk.
   * @param data Chunk of records to encode.
   * @param state Current state of the encoder.
   */
  void EncodeSeq(core::record::Chunk& data, EncodingState& state);

  /**
   * @brief Pack encoded data into a reference unit.
   * @param id Multithreading synchronization ID.
   * @param qv Encoded quality values.
   * @param read_name Encoded read names.
   * @param state Current state of the encoder.
   * @return Assembled access unit with the provided parameters.
   */
  virtual core::AccessUnit Pack(size_t id, core::QvEncoder::qv_coded qv,
                                core::AccessUnit::Descriptor read_name,
                                EncodingState& state);

  /**
   * @brief Remove redundant descriptors from the access unit based on the
   * current class type.
   * @param raw_au Access unit to be modified.
   */
  static void RemoveRedundantDescriptors(core::AccessUnit& raw_au);

  /**
   * @brief Encode all quality values in a chunk of data.
   * @param qv_coder Quality value encoder.
   * @param data Chunk of records to encode.
   * @return Encoded quality values.
   */
  static core::QvEncoder::qv_coded EncodeQVs(qv_selector* qv_coder,
                                             core::record::Chunk& data);

  /**
   * @brief Encode read names in a chunk of data.
   * @param name_coder Name encoder.
   * @param data Chunk of records to encode.
   * @return Encoded read names.
   */
  static core::AccessUnit::Descriptor EncodeNames(name_selector* name_coder,
                                                  core::record::Chunk& data);

  /**
   * @brief Constructs an EncoderStub with a flag for writing raw data.
   * @param write_raw Flag indicating if raw data should be written.
   */
  explicit EncoderStub(const bool write_raw) : ReadEncoder(write_raw) {}

  /**
   * @brief Default destructor.
   */
  ~EncoderStub() override = default;

  /**
   * @brief Process incoming chunks of data.
   * @param t Chunk of data to be processed.
   * @param id Multithreading synchronization ID.
   */
  void FlowIn(core::record::Chunk&& t, const util::Section& id) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::read::basecoder

#endif  // SRC_GENIE_READ_BASECODER_ENCODER_STUB_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
