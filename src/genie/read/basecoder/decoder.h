/**
 * @file
 * @brief Header file for the basecoder `Decoder` class.
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * This file defines the `Decoder` class used in the `genie::read::basecoder` module.
 * The `Decoder` class is responsible for decoding base sequences from access units,
 * extracting metadata, and reconstructing genomic records. It handles various scenarios
 * involving multiple template segments and different alignment cases.
 *
 * @details The `Decoder` class interacts with other modules in the GENIE framework,
 * such as the `core::AccessUnit`, to decode quality values and manage sequence alignments.
 * The decoding process includes operations like alignment restoration, segment handling,
 * and sequence mismatch correction.
 */

#ifndef SRC_GENIE_READ_BASECODER_DECODER_H_
#define SRC_GENIE_READ_BASECODER_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <array>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include "genie/core/access-unit.h"
#include "genie/core/qv-decoder.h"
#include "genie/core/record/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::basecoder {

/**
 * @brief The `Decoder` class decodes base sequences and metadata.
 *
 * The `Decoder` class processes access units and reconstructs genomic records by decoding
 * sequences, quality values, and segment metadata. It supports operations like mismatch
 * correction, segment merging, and handling multiple template segments.
 */
class Decoder {
 private:
    core::AccessUnit container;  //!< @brief Access unit container for encoded data.
    uint64_t position;           //!< @brief Current position within the access unit.
    uint64_t length;             //!< @brief Total length of the access unit.

    uint64_t recordCounter;  //!< @brief Counter to track the number of records processed.

    size_t number_template_segments;  //!< @brief Number of template segments to decode.

 public:
    /**
     * @brief Constructs a `Decoder` object.
     * @param au Access unit to decode.
     * @param segments Number of template segments to decode.
     * @param pos Starting position within the access unit.
     */
    Decoder(core::AccessUnit &&au, size_t segments, size_t pos = 0);

    /**
     * @brief Holds metadata for segments.
     */
    struct SegmentMeta {
        std::array<uint64_t, 2> position;  //!< @brief Start positions for both segments.
        std::array<uint64_t, 2> length;    //!< @brief Lengths for both segments.
        bool first1;                       //!< @brief Flag indicating if the first segment is read 1.
        uint8_t decoding_case;             //!< @brief Case identifier for decoding.
        uint8_t num_segments;              //!< @brief Number of segments.
    };

    /**
     * @brief Pulls a record from the access unit.
     * @param ref Reference identifier.
     * @param vec Vector of sequences.
     * @param meta Metadata for the segment.
     * @return The reconstructed record.
     */
    core::record::Record pull(uint16_t ref, std::vector<std::string> &&vec, const SegmentMeta &meta);

    /**
     * @brief Reads metadata for a segment.
     * @return The segment metadata.
     */
    Decoder::SegmentMeta readSegmentMeta();

    /**
     * @brief Decodes an alignment from the sequence and cigar string.
     * @param clip_offset Offset for soft clipping.
     * @param seq The sequence to decode.
     * @param cigar The CIGAR string associated with the sequence.
     * @return A tuple containing an alignment box and the corresponding record.
     */
    std::tuple<core::record::AlignmentBox, core::record::Record> decode(size_t clip_offset, std::string &&seq,
                                                                        std::string &&cigar);

    /**
     * @brief Contracts an extended CIGAR string to a regular CIGAR format.
     * @param cigar_long Extended CIGAR string.
     * @return Contracted CIGAR string.
     */
    static std::string contractECigar(const std::string &cigar_long);

    /**
     * @brief Decodes additional alignment information.
     * @param softclip_offset Offset for soft clipping.
     * @param seq The sequence to decode.
     * @param cigar The CIGAR string for alignment.
     * @param delta_pos Position delta for alignment.
     * @param state Current state of the alignment box and record.
     */
    void decodeAdditional(size_t softclip_offset, std::string &&seq, std::string &&cigar, uint16_t delta_pos,
                          std::tuple<core::record::AlignmentBox, core::record::Record> &state);

    /**
     * @brief Decodes the number of deletions.
     * @param number Number of deletions to decode.
     * @return Vector of decoded deletions.
     */
    std::vector<int32_t> numberDeletions(size_t number);

    /**
     * @brief Decodes mismatches within the sequence.
     * @param clip_offset Offset for clipping.
     * @param sequence The sequence to decode.
     * @param cigar_extended The extended CIGAR string.
     */
    void decodeMismatches(size_t clip_offset, std::string &sequence, std::string &cigar_extended);

    /**
     * @brief Decodes soft and hard clips from the sequence.
     * @param sequences The sequences to decode.
     * @param cigar_extended The extended CIGAR string.
     * @return A tuple with the start and end clip sizes.
     */
    std::tuple<size_t, size_t> decodeClips(std::vector<std::string> &sequences,
                                           std::vector<std::string> &cigar_extended);

    /**
     * @brief Clears the internal state of the decoder.
     */
    void clear();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::basecoder

#endif  // SRC_GENIE_READ_BASECODER_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
