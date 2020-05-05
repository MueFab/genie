/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SAM_RECORD_H
#define GENIE_SAM_RECORD_H

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <genie/core/cigar-tokenizer.h>

#include <map>
#include <list>
#include <vector>
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

    Record(std::string _qname, uint16_t _flag, std::string _rname, uint32_t _pos, uint8_t _mapq, std::string _cigar,
           std::string _rnext, uint32_t _pnext, int32_t _tlen, std::string _seq, std::string _qual);

    Record();

    explicit Record(const std::string &string);

    explicit Record(const std::string &_qname, const std::string &string);

    const std::string &getQname() const;

    std::string &&moveQname();

    uint16_t getFlags() const;

    bool checkFlag(FlagPos f) const;

    const std::string &getRname() const;

    std::string &&moveRname();

    uint32_t getPos() const;

    uint8_t getMapQ() const;

    const std::string &getCigar() const;

    std::string &&moveCigar();

    const std::string &getRnext() const;

    std::string &&moveRnext();

    uint32_t getPnext() const;

    int32_t getTlen() const;

    const std::string &getSeq() const;

    std::string &&moveSeq();

    const std::string &getQual() const;

    std::string &&moveQual();

    std::string toString() const;

//    const std::string &getReverseSeq() const;

    void checkValuesUsingRegex() const;

    void checkValuesUsingCondition() const;

    bool isPrimaryLine() const;

    bool isUnmapped() const;

    bool isNextUnmapped() const;

    bool isPairOf(Record &other) const;
};

// ---------------------------------------------------------------------------------------------------------------------

class ReadTemplate{
   public:
    enum class Index : uint8_t {
        SINGLE_UNMAPPED = 0,
        SINGLE_MAPPED = 1,
        PAIR_FIRST_PRIMARY = 2,
        PAIR_LAST_PRIMARY = 3,
        PAIR_FIRST_NONPRIMARY = 4,
        PAIR_LAST_NONPRIMARY = 5,
        UNKNOWN = 6,
        TOTAL_TYPES = 7,
    };

   private:
    std::vector<std::list<Record>> data;
    bool is_empty = false;

   public:
    std::string qname;

    ReadTemplate();
    explicit ReadTemplate(Record&& rec);

    void initializeData();
    void addRecord(Record&& rec);

    bool isUnmapped();
    bool isSingle();
    bool isPair();
    bool isUnknown();

    bool isValid();
    bool getSamRecords(std::list<std::list<Record>> & sam_recs);
};

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_RECORD_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------