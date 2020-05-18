/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SAM_RECORD_H
#define GENIE_SAM_RECORD_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/cigar-tokenizer.h>
#include <genie/core/stats/perf-stats.h>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

const core::CigarFormatInfo& getSAMCigarInfo();

// ---------------------------------------------------------------------------------------------------------------------

class Record {
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
    struct Stats {
        bool active{true};
        size_t qname{};
        size_t flag{};
        size_t rname{};
        size_t pos{};
        size_t mapq{};
        size_t cigar{};
        size_t rnext{};
        size_t pnext{};
        size_t tlen{};
        size_t seq{};
        size_t qual{};
        size_t opt{};

        void add(const Stats& s) {
            qname += s.qname;
            flag += s.flag;
            rname += s.rname;
            pos += s.pos;
            mapq += s.mapq;
            cigar += s.cigar;
            rnext += s.rnext;
            pnext += s.pnext;
            tlen += s.tlen;
            seq += s.seq;
            qual += s.qual;
            opt += s.opt;
        }

        size_t total() const {
            return qname + flag + rname + pos + mapq + cigar + rnext + pnext + tlen + seq + qual + opt;
        }
    };
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

    Record(std::string _qname, uint16_t _flag, std::string _rname, uint32_t _pos, uint8_t _mapq, std::string _cigar,
           std::string _rnext, uint32_t _pnext, int32_t _tlen, std::string _seq, std::string _qual);

    Record();

    explicit Record(const std::string& string, Stats& s);

    void check() const;

    const std::string& getQname() const;
    std::string&& moveQname();
    uint16_t getFlags() const;
    bool checkFlag(FlagPos f) const;
    const std::string& getRname() const;
    std::string&& moveRname();
    uint32_t getPos() const;
    uint8_t getMapQ() const;

    const std::string& getCigar() const;
    std::string&& moveCigar();
    const std::string& getRnext() const;
    std::string&& moveRnext();
    uint32_t getPnext() const;
    int32_t getTlen() const;
    const std::string& getSeq() const;
    std::string&& moveSeq();
    const std::string& getQual() const;
    std::string&& moveQual();
    std::string toString() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_RECORD_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
