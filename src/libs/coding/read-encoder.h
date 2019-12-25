#ifndef GENIE_READ_ENCODER_H
#define GENIE_READ_ENCODER_H

#include <format/sam/sam-record.h>
#include <memory>
#include <string>
#include <vector>
#include "format/mpegg_rec/mpegg-record.h"
#include "mpegg-raw-au.h"

namespace lae {
class LocalAssemblyReadEncoder {
   private:
    std::unique_ptr<MpeggRawAu> container;
    int32_t pos;
    uint32_t readCounter;

    void encodeFirstSegment(const format::mpegg_rec::MpeggRecord *rec);

    void encodeAdditionalSegment(const format::mpegg_rec::MpeggRecord *rec, size_t index);

    struct ClipInformation {
        std::array<std::string, 2> softClips;
        std::array<size_t, 2> hardClips = {0, 0};
    };

    void codeVariants(const std::string &read, const std::string &cigar, const std::string &ref,
                      format::mpegg_rec::MpeggRecord::ClassType type, ClipInformation *clips);

    bool encodeClips(const ClipInformation &inf, bool last);

    void encodeClips(const ClipInformation &inf1, const ClipInformation &inf2);

   public:
    explicit LocalAssemblyReadEncoder();

    void add(const format::mpegg_rec::MpeggRecord *rec, const std::string &ref1, const std::string &ref2);

    std::unique_ptr<MpeggRawAu> pollStreams();
};
}  // namespace lae

#endif  // GENIE_READ_ENCODER_H
