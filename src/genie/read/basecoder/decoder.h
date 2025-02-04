/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_BASECODER_DECODER_H_
#define SRC_GENIE_READ_BASECODER_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <array>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include "genie/core/access_unit.h"
#include "genie/core/qv_decoder.h"
#include "genie/core/record/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace basecoder {

/**
 * @brief
 */
class Decoder {
 private:
    core::AccessUnit container;  //!< @brief
    uint64_t position;           //!< @brief
    uint64_t length;             //!< @brief

    uint64_t recordCounter;  //!< @brief

    size_t number_template_segments;  //!< @brief

 public:
    /**
     * @brief
     * @param au
     * @param segments
     * @param pos
     */
    Decoder(core::AccessUnit &&au, size_t segments, size_t pos = 0);

    /**
     * @brief
     */
    struct SegmentMeta {
        std::array<uint64_t, 2> position;  //!< @brief
        std::array<uint64_t, 2> length;    //!< @brief
        bool first1;                       //!< @brief
        uint8_t decoding_case;             //!< @brief
        uint8_t num_segments;              //!< @brief
    };

    /**
     * @brief
     * @param ref
     * @param vec
     * @param meta
     * @return
     */
    core::record::Record pull(uint16_t ref, std::vector<std::string> &&vec, const SegmentMeta &meta);

    /**
     * @brief
     * @return
     */
    Decoder::SegmentMeta readSegmentMeta();

    /**
     * @brief
     * @param clip_offset
     * @param seq
     * @param cigar
     * @return
     */
    std::tuple<core::record::AlignmentBox, core::record::Record> decode(size_t clip_offset, std::string &&seq,
                                                                        std::string &&cigar);

    /**
     * @brief
     * @param cigar_long
     * @return
     */
    static std::string contractECigar(const std::string &cigar_long);

    /**
     * @brief
     * @param softclip_offset
     * @param seq
     * @param cigar
     * @param delta_pos
     * @param state
     */
    void decodeAdditional(size_t softclip_offset, std::string &&seq, std::string &&cigar, uint16_t delta_pos,
                          std::tuple<core::record::AlignmentBox, core::record::Record> &state);

    /**
     * @brief
     * @param number
     * @return
     */
    std::vector<int32_t> numberDeletions(size_t number);

    /**
     * @brief
     * @param clip_offset
     * @param sequence
     * @param cigar_extended
     */
    void decodeMismatches(size_t clip_offset, std::string &sequence, std::string &cigar_extended);

    /**
     * @brief
     * @param sequences
     * @param cigar_extended
     * @return
     */
    std::tuple<size_t, size_t> decodeClips(std::vector<std::string> &sequences,
                                           std::vector<std::string> &cigar_extended);

    /**
     * @brief
     */
    void clear();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace basecoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_BASECODER_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
