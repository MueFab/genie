/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_R_DECODER_H
#define GENIE_R_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>

#include <genie/core/access-unit.h>
#include <genie/core/record/record.h>
#include <qv-decoder.h>
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace basecoder {

/**
 *
 */
class Decoder {
   private:
    core::AccessUnit container;  //!<
    uint64_t position;           //!<
    uint64_t length;             //!<

    uint64_t recordCounter;  //!<

    size_t number_template_segments;  //!<

   public:
    /**
     *
     * @param au
     * @param segments
     */
    Decoder(core::AccessUnit &&au, size_t segments, size_t pos = 0);

    /**
     *
     * @param ref
     * @param vec
     * @return
     */
    core::record::Record pull(uint16_t ref, std::vector<std::string> &&vec);

    /**
     *
     */
    struct SegmentMeta {
        uint64_t position;  //!<
        uint64_t length;    //!<
    };

    /**
     *
     */
    struct SegmentDecoded {
        core::record::Record record;  //!<
        uint64_t length;              //!<
    };

    /**
     *
     * @return
     */
    std::vector<SegmentMeta> readSegmentMeta();

    /**
     *
     * @param clip_offset
     * @param seq
     * @param cigar
     * @return
     */
    std::tuple<core::record::AlignmentBox, core::record::Record> decode(size_t clip_offset, std::string &&seq,
                                                                        std::string &&cigar);

    /**
     *
     * @param cigar_long
     * @return
     */
    static std::string contractECigar(const std::string &cigar_long);

    /**
     *
     * @param softclip_offset
     * @param seq
     * @param cigar
     * @param state
     */
    void decodeAdditional(size_t softclip_offset, std::string &&seq, std::string &&cigar,
                          std::tuple<core::record::AlignmentBox, core::record::Record> &state);

    /**
     *
     * @param number
     * @return
     */
    std::vector<int32_t> numberDeletions(size_t number);

    /**
     *
     * @param clip_offset
     * @param sequence
     * @param cigar_extended
     */
    void decodeMismatches(size_t clip_offset, std::string &sequence, std::string &cigar_extended);

    /*+
     *
     * @param sequences
     * @param cigar_extended
     * @return
     */
    std::tuple<size_t, size_t> decodeClips(std::vector<std::string> &sequences,
                                           std::vector<std::string> &cigar_extended);

    /**
     *
     */
    void clear();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace basecoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------