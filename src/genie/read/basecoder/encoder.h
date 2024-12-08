/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.h
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for the basecoder `Encoder` class.
 *
 * This file defines the `Encoder` class, which is used for encoding aligned
 * genomic reads into a specific data format using the GENIE framework. The
 * class handles different operations such as encoding CIGAR strings, soft and
 * hard clips, and various alignment manipulations.
 *
 * @details The `Encoder` class encapsulates the functionality for transforming
 * read data into a compressed format. It operates on records defined in the
 * GENIE core and utilizes the `core::AccessUnit` structure for managing encoded
 * data streams.
 */

#ifndef SRC_GENIE_READ_BASECODER_ENCODER_H_
#define SRC_GENIE_READ_BASECODER_ENCODER_H_

// -----------------------------------------------------------------------------

#include <genie/core/read_encoder.h>
#include <genie/util/stop_watch.h>

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/access_unit.h"
#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/core/record/record.h"

// -----------------------------------------------------------------------------

namespace genie::read::basecoder {

/**
 * @brief The Encoder class handles the encoding of aligned genomic reads.
 *
 * The Encoder class provides functionality for encoding genomic reads by
 * working with the GENIE core record classes. It manages various encoding
 * states and operations for transforming the raw read data into a structured,
 * compressed format.
 */
class Encoder {
  core::AccessUnit
      container_;  //!< @brief Container to store the encoded access unit.
  uint64_t pos_;   //!< @brief Position for the next record.
  uint32_t
      read_counter_;  //!< @brief Counter for the number of processed reads.

  /**
   * @brief Encodes the first segment of a record.
   * @param rec The record to encode.
   */
  void EncodeFirstSegment(const core::record::Record& rec);

  /**
   * @brief Encodes additional segments of a record.
   * @param length Length of the additional segment.
   * @param split_rec Reference to the record split segment.
   * @param first1 Flag indicating if this is the first additional segment.
   */
  void EncodeAdditionalSegment(
      size_t length, const core::record::alignment_split::SameRec& split_rec,
      bool first1);

  /**
   * @brief Structure to hold clipping information.
   */
  struct ClipInformation {
    std::array<std::string, 2> soft_clips;  //!< @brief Soft clipping sequences.
    std::array<size_t, 2> hard_clips = {
        {0, 0}};  //!< @brief Number of bases hard clipped.
  };

  /**
   * @brief Encodes a CIGAR string.
   * @param read Read sequence.
   * @param cigar CIGAR string.
   * @param ref Reference sequence.
   * @param type Record class type.
   * @return Clipping information.
   */
  ClipInformation EncodeCigar(const std::string& read, const std::string& cigar,
                              const std::string& ref,
                              core::record::ClassType type);

  /**
   * @brief Encodes a single clipping.
   * @param inf Clipping information.
   * @param last Indicates if this is the last clipping.
   * @return True if clipping was successfully encoded, false otherwise.
   */
  bool EncodeSingleClip(const ClipInformation& inf, bool last);

  /**
   * @brief Encodes clipping information.
   * @param clips Pair of clipping information for paired reads.
   */
  void EncodeClips(const std::pair<ClipInformation, ClipInformation>& clips);

  /**
   * @brief Holds the current state of the encoding process.
   */
  struct CodingState {
    CodingState(const std::string& read_seq, const std::string& ref_name,
                core::record::ClassType c_type);
    size_t count;          //!< @brief Current count of operations.
    size_t read_pos;       //!< @brief Position in the read.
    size_t ref_offset;     //!< @brief Offset in the reference sequence.
    size_t last_mismatch;  //!< @brief Last mismatch position.
    bool is_right_clip;    //!< @brief Indicates if currently processing a
                           //!< right clip.

    const std::string& read;             //!< @brief The read sequence.
    const std::string& ref;              //!< @brief The reference sequence.
    const core::record::ClassType type;  //!< @brief Type of the record.
    ClipInformation
        clips;  //!< @brief Clipping information for the current read.
  };

  /**
   * @brief Encodes an insertion event.
   * @param state Current encoding state.
   */
  void EncodeInsertion(CodingState& state);

  /**
   * @brief Encodes a deletion event.
   * @param state Current encoding state.
   */
  void EncodeDeletion(CodingState& state);

  /**
   * @brief Encodes a hard clip event.
   * @param state Current encoding state.
   */
  static void EncodeHardClip(CodingState& state);

  /**
   * @brief Encodes a soft clip event.
   * @param state Current encoding state.
   */
  static void EncodeSoftClip(CodingState& state);

  /**
   * @brief Encodes a substitution event.
   * @param state Current encoding state.
   */
  void EncodeSubstitution(CodingState& state);

  /**
   * @brief Encodes a splice event.
   * @param state Current encoding state.
   */
  static void EncodeSplice(const CodingState& state);

  /**
   * @brief Encodes a match event.
   * @param state Current encoding state.
   */
  void EncodeMatch(CodingState& state);

  /**
   * @brief Updates the operation count based on a CIGAR character.
   * @param cigar_char CIGAR character.
   * @param state Current encoding state.
   * @return True if count updated successfully, false otherwise.
   */
  static bool UpdateCount(char cigar_char, CodingState& state);

  /**
   * @brief Encodes a single CIGAR token.
   * @param cigar_char CIGAR character.
   * @param state Current encoding state.
   */
  void EncodeCigarToken(char cigar_char, CodingState& state);

  /**
   * @brief Extracts paired alignment information from a record.
   * @param rec The record to process.
   * @return Reference to the paired alignment.
   */
  [[nodiscard]] static const core::record::alignment_split::SameRec&
  ExtractPairedAlignment(const core::record::Record& rec);

 public:
  /**
   * @brief Initializes the encoder with a starting mapping position.
   * @param starting_mapping_pos The starting position in the reference.
   */
  explicit Encoder(uint64_t starting_mapping_pos);

  /**
   * @brief Adds a record to the encoder.
   * @param rec Record to encode.
   * @param ref1 First reference sequence.
   * @param ref2 Second reference sequence.
   */
  void Add(const core::record::Record& rec, const std::string& ref1,
           const std::string& ref2);

  /**
   * @brief Moves the encoded access unit out of the encoder.
   * @return Encoded access unit.
   */
  core::AccessUnit&& MoveStreams();
};

// -----------------------------------------------------------------------------

}  // namespace genie::read::basecoder

#endif  // SRC_GENIE_READ_BASECODER_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
