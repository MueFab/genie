#ifndef GENIE_READ_DECODER_H
#define GENIE_READ_DECODER_H

#include <string>
#include <vector>

#include <backbone/mpegg_rec/mpegg-record.h>
#include <memory>
#include "backbone/mpegg-raw-au.h"
namespace genie {
namespace mpegg_read {
class LocalAssemblyReadDecoder {
   private:
    genie::MpeggRawAu container;
    uint64_t position;
    uint64_t length;

    uint64_t recordCounter;

    size_t number_template_segments;

   public:
    LocalAssemblyReadDecoder(genie::MpeggRawAu &&au, size_t segments);
    genie::mpegg_rec::MpeggRecord pull(uint16_t ref, std::vector<std::string> &&vec);

    struct SegmentMeta {
        uint64_t position;
        uint64_t length;
    };

    struct SegmentDecoded {
        genie::mpegg_rec::MpeggRecord record;
        uint64_t length;
    };

    std::vector<SegmentMeta> readSegmentMeta();

    std::tuple<genie::mpegg_rec::AlignmentContainer, genie::mpegg_rec::MpeggRecord> decode(size_t clip_offset,
                                                                                           std::string &&seq,
                                                                                           std::string &&cigar);

    static std::string contractECigar(const std::string &cigar_long);

    void decodeAdditional(size_t softclip_offset, std::string &&seq, std::string &&cigar,
                          std::tuple<genie::mpegg_rec::AlignmentContainer, genie::mpegg_rec::MpeggRecord> &state);

    std::vector<int32_t> numberDeletions(size_t number);

    void decodeMismatches(size_t clip_offset, std::string &sequence, std::string &cigar_extended);

    std::tuple<size_t, size_t> decodeClips(std::vector<std::string> &sequences,
                                           std::vector<std::string> &cigar_extended);
};
}  // namespace mpegg_read
}  // namespace genie
#endif  // GENIE_READ_DECODER_H
