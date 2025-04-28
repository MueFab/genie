/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.h
 * @brief Implementation of the Decoder class for decoding base sequences and
 * metadata.
 *
 * This file contains the implementation of the Decoder class, which is
 * responsible for decoding base sequences from access units, extracting
 * metadata, and reconstructing genomic records. It handles various scenarios
 * involving multiple template segments and different alignment cases.
 *
 * @details The Decoder class interacts with other modules in the GENIE
 * framework, such as the core::AccessUnit, to decode quality values and manage
 * sequence alignments. The decoding process includes operations like alignment
 * restoration, segment handling, and sequence mismatch correction.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_BASECODER_DECODER_H_
#define SRC_GENIE_READ_BASECODER_DECODER_H_

// -----------------------------------------------------------------------------

#include <array>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "genie/core/access_unit.h"
#include "genie/core/record/record.h"

// -----------------------------------------------------------------------------

namespace genie::read::basecoder {

/**
 * @brief The `Decoder` class decodes base sequences and metadata.
 *
 * The `Decoder` class processes access units and reconstructs genomic records
 * by decoding sequences, quality values, and segment metadata. It supports
 * operations like mismatch correction, segment merging, and handling multiple
 * template segments.
 */
class Decoder {
  /// Access unit container for encoded data.
  core::AccessUnit container_;

  /// Current position within the access unit.
  uint64_t position_;

  /// Total length of the access unit.
  uint64_t length_;

  /// Counter to track the number of records processed.
  uint64_t record_counter_;

  /// Number of template segments to Decode.
  size_t number_template_segments_;

 public:
  /**
   * @brief Constructs a `Decoder` object.
   * @param au Access unit to Decode.
   * @param segments Number of template segments to Decode.
   * @param pos Starting position within the access unit.
   */
  Decoder(core::AccessUnit&& au, size_t segments, size_t pos = 0);

  /**
   * @brief Holds information about soft and hard clips.
   */
  struct Clips {
    /// Hard clips for both segments and both ends.
    std::array<std::array<size_t, 2>, 2> hard_clips = {{{0, 0}, {0, 0}}};

    /// Soft clips for both segments and both ends.
    std::array<std::array<std::string, 2>, 2> soft_clips;
  };

  /**
   * @brief Holds metadata for segments.
   */
  struct SegmentMeta {
    /// Start positions for both segments.
    std::array<uint64_t, 2> position;

    /// Lengths for both segments.
    std::array<uint64_t, 2> length;

    /// Flag indicating if the first segment is read 1.
    bool first1;

    /// Case identifier for decoding.
    uint8_t decoding_case;

    /// Number of segments.
    uint8_t num_segments;

    /// Clipping information for the segments.
    Clips clips;
  };

  /**
   * @brief Decodes the clips from the access unit.
   * @return The decoded clips.
   */
  Clips DecodeClips();

  /**
   * @brief Applies clips to the sequences and CIGAR strings.
   * @param clips The clips to apply.
   * @param sequences Vector of sequences.
   * @param cigar_extended Vector of extended CIGAR strings.
   * @return A tuple containing the soft clip offsets for both segments.
   */
  static std::tuple<size_t, size_t> ApplyClips(
      const Clips& clips, std::vector<std::string>& sequences,
      std::vector<std::string>& cigar_extended);

  /**
   * @brief Pulls a record from the access unit.
   * @param ref Reference identifier.
   * @param vec Vector of sequences.
   * @param meta Metadata for the segment.
   * @return The reconstructed record.
   */
  core::record::Record Pull(uint16_t ref, std::vector<std::string>&& vec,
                            const SegmentMeta& meta);

  /**
   * @brief Reads metadata for a segment.
   * @return The segment metadata.
   */
  SegmentMeta ReadSegmentMeta();

  /**
   * @brief Decodes an alignment from the sequence and cigar string.
   * @param clip_offset Offset for soft clipping.
   * @param seq The sequence to Decode.
   * @param cigar The CIGAR string associated with the sequence.
   * @return A tuple containing an alignment box and the corresponding record.
   */
  std::tuple<core::record::AlignmentBox, core::record::Record> Decode(
      size_t clip_offset, std::string&& seq, std::string&& cigar);

  /**
   * @brief Contracts an extended CIGAR string to a regular CIGAR format.
   * @param cigar_long Extended CIGAR string.
   * @return Contracted CIGAR string.
   */
  static std::string ContractECigar(const std::string& cigar_long);

  /**
   * @brief Decodes additional alignment information.
   * @param softclip_offset Offset for soft clipping.
   * @param seq The sequence to Decode.
   * @param cigar The CIGAR string for alignment.
   * @param delta_pos Position delta for alignment.
   * @param state Current state of the alignment box and record.
   */
  void DecodeAdditional(
      size_t softclip_offset, std::string&& seq, std::string&& cigar,
      uint16_t delta_pos,
      std::tuple<core::record::AlignmentBox, core::record::Record>& state);

  /**
   * @brief Decodes the number of deletions.
   * @param number Number of deletions to Decode.
   * @return Vector of decoded deletions.
   */
  std::vector<int32_t> NumberDeletions(size_t number);

  /**
   * @brief Decodes mismatches within the sequence.
   * @param clip_offset Offset for clipping.
   * @param sequence The sequence to Decode.
   * @param cigar_extended The extended CIGAR string.
   */
  void DecodeMismatches(size_t clip_offset, std::string& sequence,
                        std::string& cigar_extended);

  /**
   * @brief Clears the internal state of the decoder.
   */
  void Clear();
};

// -----------------------------------------------------------------------------

}  // namespace genie::read::basecoder

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_BASECODER_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
