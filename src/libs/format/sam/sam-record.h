#ifndef GENIE_SAM_RECORD_H
#define GENIE_SAM_RECORD_H

#include <util/tokenize.h>
#include "sam-tag.h"

namespace format {
namespace sam {
class SamRecord {
   private:
    std::string qname;
    uint16_t flag;
    std::string rname;
    uint32_t pos;
    uint8_t mapq;
    std::string cigar;
    std::string rnext;
    uint32_t pnext;
    int32_t tlen;
    std::string seq;
    std::string qual;
    // TODO: Tags
   public:
    enum class FlagPos : uint16_t {
        MULTI_SEGMENT_TEMPLATE = 0,
        PROPERLY_ALIGNED = 1,
        SEGMENT_UNMAPPED = 2,
        NEXT_SEGMENT_UNMAPPED = 3,
        SEQ_REVERSE = 4,
        NEXT_SEQ_REVERSE = 5,
        FIRST_SEGMENT = 6,
        LAST_SEGMENT = 7,
        SECONDARY_ALIGNMENT = 8,
        QUALITY_FAIL = 9,
        PCR_DUPLICATE = 10,
        SUPPLEMENTARY_ALIGNMENT = 11
    };

    SamRecord(std::string _qname, uint16_t _flag, std::string _rname, uint32_t _pos, uint8_t _mapq, std::string _cigar,
              std::string _rnext, uint32_t _pnext, int32_t _tlen, std::string _seq, std::string _qual);

    explicit SamRecord(const std::string& string);

    void check() const;

    const std::string& getQname() const;
    std::string&& moveQname() { return std::move(qname); }
    uint16_t getFlags() const;
    bool checkFlag(FlagPos f) const;
    const std::string& getRname() const;
    std::string&& moveRname() { return std::move(rname); }
    uint32_t getPos() const;
    uint8_t getMapQ() const;

    const std::string& getCigar() const;
    std::string&& moveCigar() { return std::move(cigar); }
    const std::string& getRnext() const;
    std::string&& moveRnext() { return std::move(rnext); }
    uint32_t getPnext() const;
    int32_t getTlen() const;
    const std::string& getSeq() const;
    std::string&& moveSeq() { return std::move(seq); }
    const std::string& getQual() const;
    std::string&& moveQual() { return std::move(qual); }
    std::string toString() const;
};
}  // namespace sam
}  // namespace format

#endif  // GENIE_SAM_RECORD_H
