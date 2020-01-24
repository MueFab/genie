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

#include <genie/core/access-unit-raw.h>
#include <genie/core/record/record.h>
#include <qv-decoder.h>
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace basecoder {

// ---------------------------------------------------------------------------------------------------------------------

class Decoder {
   private:
    core::AccessUnitRaw container;
    uint64_t position;
    uint64_t length;

    uint64_t recordCounter;

    size_t number_template_segments;

   public:
    Decoder(core::AccessUnitRaw &&au, size_t segments);
    core::record::Record pull(uint16_t ref, core::QVDecoder &qvdecoder, std::vector<std::string> &&vec);

    struct SegmentMeta {
        uint64_t position;
        uint64_t length;
    };

    struct SegmentDecoded {
        core::record::Record record;
        uint64_t length;
    };

    std::vector<SegmentMeta> readSegmentMeta();

    std::tuple<core::record::AlignmentBox, core::record::Record> decode(size_t clip_offset, core::QVDecoder &qvdecoder,
                                                                        std::string &&seq, std::string &&cigar);

    static std::string contractECigar(const std::string &cigar_long);

    void decodeAdditional(size_t softclip_offset, core::QVDecoder &qvdecoder, std::string &&seq, std::string &&cigar,
                          std::tuple<core::record::AlignmentBox, core::record::Record> &state);

    std::vector<int32_t> numberDeletions(size_t number);

    void decodeMismatches(size_t clip_offset, std::string &sequence, std::string &cigar_extended);

    std::tuple<size_t, size_t> decodeClips(std::vector<std::string> &sequences,
                                           std::vector<std::string> &cigar_extended);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace basecoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------