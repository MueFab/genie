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

/**
 *
 * @return
 */
const core::CigarFormatInfo& getSAMCigarInfo();

/**
 *
 */
class Record {
   private:
    std::string qname;  //!<
    uint16_t flag;      //!<
    std::string rname;  //!<
    uint32_t pos;       //!<
    uint8_t mapq;       //!<
    std::string cigar;  //!<
    std::string rnext;  //!<
    uint32_t pnext;     //!<
    int32_t tlen;       //!<
    std::string seq;    //!<
    std::string qual;   //!<
                        // TODO: Tags
   public:
    struct Stats {
        bool active{true};  //!<
        size_t qname{};     //!<
        size_t flag{};      //!<
        size_t rname{};     //!<
        size_t pos{};       //!<
        size_t mapq{};      //!<
        size_t cigar{};     //!<
        size_t rnext{};     //!<
        size_t pnext{};     //!<
        size_t tlen{};      //!<
        size_t seq{};       //!<
        size_t qual{};      //!<
        size_t opt{};       //!<

        /**
         *
         * @param s
         */
        void add(const Stats& s);

        /**
         *
         * @return
         */
        size_t total() const;
    };

    /**
     *
     */
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
     *
     * @param _qname
     * @param _flag
     * @param _rname
     * @param _pos
     * @param _mapq
     * @param _cigar
     * @param _rnext
     * @param _pnext
     * @param _tlen
     * @param _seq
     * @param _qual
     */
    Record(std::string _qname, uint16_t _flag, std::string _rname, uint32_t _pos, uint8_t _mapq, std::string _cigar,
           std::string _rnext, uint32_t _pnext, int32_t _tlen, std::string _seq, std::string _qual);

    /**
     *
     */
    Record();

    /**
     *
     * @param string
     * @param s
     */
    explicit Record(const std::string& string, Stats& s);

    /**
     *
     */
    void check() const;

    /**
     *
     * @return
     */
    const std::string& getQname() const;

    /**
     *
     * @return
     */
    std::string&& moveQname();

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
    const std::string& getRname() const;

    /**
     *
     * @return
     */
    std::string&& moveRname();

    /**
     *
     * @return
     */
    uint32_t getPos() const;

    /**
     *
     * @return
     */
    uint8_t getMapQ() const;

    /**
     *
     * @return
     */
    const std::string& getCigar() const;

    /**
     *
     * @return
     */
    std::string&& moveCigar();

    /**
     *
     * @return
     */
    const std::string& getRnext() const;

    /**
     *
     * @return
     */
    std::string&& moveRnext();

    /**
     *
     * @return
     */
    uint32_t getPnext() const;

    /**
     *
     * @return
     */
    int32_t getTlen() const;

    /**
     *
     * @return
     */
    const std::string& getSeq() const;

    /**
     *
     * @return
     */
    std::string&& moveSeq();

    /**
     *
     * @return
     */
    const std::string& getQual() const;

    /**
     *
     * @return
     */
    std::string&& moveQual();

    /**
     *
     * @return
     */
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
