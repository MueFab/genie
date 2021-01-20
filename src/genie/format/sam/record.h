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

    /**
     * @brief Default constructor of SAM record
     */
    Record();


    /**
     *
     * @param _qname: Query template name (QNAME)
     * @param _flag: Flag (FLAG)
     * @param _rname: Reference name (RNAME)
     * @param _pos: 1-based leftmost mapping Position (POS)
     * @param _mapq: Mapping quality (MAPQ)
     * @param _cigar: CIGAR string
     * @param _rnext: Reference sequence name of the primary alignment of the next read in the template (RNEXT)
     * @param _pnext: 1-based Position of the primary alignment of the next read in the template (PNEXT)
     * @param _tlen: signed observed Template length (TLEN)
     * @param _seq: Segment sequence (SEQ)
     * @param _qual: ASCII of base QUALity plus 33 (QUAL)
     */
    Record(std::string _qname, uint16_t _flag, std::string _rname, uint32_t _pos, uint8_t _mapq, std::string _cigar,
           std::string _rnext, uint32_t _pnext, int32_t _tlen, std::string _seq, std::string _qual);

    /**
     *
     * @brief Parse and convert string into SAM record
     *
     * @param string: SAM record as string
     */
    explicit Record(const std::string &string);
    explicit Record(const std::string &_qname, const std::string &string);

    /**
     * @brief Get QNAME
     *
     * @return QNAME
     */
    const std::string &getQname() const;

    /**
     *
     * @return
     */
    std::string &&moveQname();

    /**
     *
     * @return
     */
    uint16_t getFlags() const;

    /**
     *
     * @param f
     * @return
     */
    bool checkFlag(FlagPos f) const;

    /**
     *
     * @return
     */
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

    /**
     * @brief Check if current record is the first segment in the template
     *
     * @return
     */
    bool isFirstRead() const;

    /**
     * @brief Check if current record is the last segment in the template
     *
     * @return
     */
    bool isLastRead() const;

    /**
     * @brief Check if the current record is paired-end reads
     *
     * @return
     */
    bool isPairedEnd() const;

    /**
     * @brief Check if current record is primary line in the template
     *
     * @return
     */
    bool isPrimaryLine() const;

    /**
     * @brief Check if the current record is unmapped
     *
     * @return
     */
    bool isUnmapped() const;

    /**
     * @brief Check if the next read in the template is unmapped
     *
     * @return
     */
    bool isNextUnmapped() const;

    /**
     * @brief Check if the current record is pair of sam record other
     *
     * @param other: other sam record
     * @return
     */
    bool isPairOf(Record &other) const;

    void setSeq(const std::string& _seq);
};

// ---------------------------------------------------------------------------------------------------------------------

class ReadTemplate{
   public:
    enum class Index : uint8_t {
        SINGLE_UNMAPPED = 0,
        SINGLE_MAPPED = 1,
        PAIR_FIRST= 2,
        PAIR_LAST = 3,
        UNKNOWN = 4,
        TOTAL_TYPES = 5,
    };

   private:
    std::string qname;
    std::vector<std::list<Record>> data;

    /**
     * @brief initialize length of data
     *
     */
    void initializeData();

   public:

    /**
     * @brief Default constructor of ReadTemplate
     *
     */
    ReadTemplate();

    /**
     * @brief Create ReadTemplate based on sam record
     *
     * @param rec
     */
    explicit ReadTemplate(Record&& rec);

    /**
     * @brief Get QNAME
     *
     * @return
     */
    const std::string& getQname();

    /**
     * @brief Add sam record
     *
     * @param rec
     */
    void addRecord(Record&& rec);

    /**
     * @brief Check if sam records in ReadTemplate are unmapped
     *
     * @return
     */
    bool isUnmapped();

    /**
     * @brief Check if sam records in ReadTemplate are single-end reads
     *
     * @return
     */
    bool isSingle();

    /**
     * @brief Check if sam records in ReadTemplate are paired-end reads
     *
     * @return
     */
    bool isPair();

    /**
     * @brief Check if sam records in ReadTemplate cannot be categorized
     *
     * @return
     */
    bool isUnknown();

    /**
     * @brief Check if sam records are valid and does not belongs to 2 or more different category
     *
     * @return
     */
    bool isValid();

    /**
     * @brief Move all sam records contained to the 2 dimensional list
     *
     * @param sam_recs
     * @return
     */
    bool getRecords(std::list<std::list<Record>> & sam_recs);
};

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_RECORD_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------