/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_SAM_RECORD_H_
#define SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_SAM_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <htslib/sam.h>
#include <string>
#include <vector>
// #include <list>
// #include <genie/core/record/record.h>

// ---------------------------------------------------------------------------------------------------------------------

#include "apps/genie/transcode-sam/sam/sam_to_mgrec/program-options.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace transcode_sam {
namespace sam {
namespace sam_to_mgrec {

/**
 * @brief
 */
class SamRecord {
 public:
    std::string qname;  //!< @brief Query template name
    uint16_t flag;      //!< @brief Flag
    int32_t rid;        //!< @brief Reference sequence ID
    uint32_t pos;       //!< @brief Position
    uint8_t mapq;       //!< @brief Mapping Quality
    std::string cigar;  //!< @brief CIGAR
    int32_t mate_rid;   //!< @brief Mate reference sequence ID
    uint32_t mate_pos;  //!< @brief Mate position
    std::string seq;    //!< @brief Read sequence
    std::string qual;   //!< @brief

 public:
    /**
     * @brief
     * @param rec
     * @return
     */
    bool operator==(const SamRecord& rec) const {
        return qname == rec.qname && flag == rec.flag && rid == rec.rid && pos == rec.pos && mapq == rec.mapq &&
               cigar == rec.cigar && mate_rid == rec.mate_rid && mate_pos == rec.mate_pos && seq == rec.seq &&
               qual == rec.qual;
    }

    /**
     * @brief
     * @param int_base
     * @return
     */
    static char fourBitBase2Char(uint8_t int_base);

    /**
     * @brief
     * @param sam_alignment
     * @return
     */
    static std::string getCigarString(bam1_t* sam_alignment);

    /**
     * @brief
     * @param sam_alignment
     * @return
     */
    static std::string getSeqString(bam1_t* sam_alignment);

    /**
     * @brief
     * @param sam_alignment
     * @return
     */
    static std::string getQualString(bam1_t* sam_alignment);

    /**
     * @brief
     * @param token
     * @return
     */
    static char convertCigar2ECigarChar(char token);

    /**
     * @brief
     * @param token
     * @return
     */
    static int stepSequence(char token);

    /**
     * @brief
     * @param cigar
     * @param seq
     * @return
     */
    static std::string convertCigar2ECigar(const std::string& cigar, const std::string& seq);

    /**
     * @brief
     */
    SamRecord();

    /**
     * @brief
     * @param sam_alignment
     */
    explicit SamRecord(bam1_t* sam_alignment);

    /**
     * @brief
     * @return
     */
    const std::string& getQname();

    /**
     * @brief
     * @return
     */
    std::string&& moveQname();

    /**
     * @brief
     * @return
     */
    uint16_t getFlag() const;

    /**
     * @brief
     * @return
     */
    int32_t getRID() const;

    /**
     * @brief
     * @return
     */
    uint32_t getPos() const;

    /**
     * @brief
     * @return
     */
    uint8_t getMapq() const;

    /**
     * @brief
     * @return
     */
    const std::string& getCigar() const;

    /**
     * @brief
     * @return
     */
    std::string&& moveCigar();

    /**
     * @brief
     * @return
     */
    std::string getECigar() const;

    /**
     * @brief
     * @return
     */
    int32_t getMRID() const;

    /**
     * @brief
     * @return
     */
    uint32_t getMPos() const;

    /**
     * @brief
     * @return
     */
    const std::string& getSeq() const;

    /**
     * @brief
     * @return
     */
    std::string&& moveSeq();

    /**
     * @brief
     * @param _seq
     */
    void setSeq(std::string&& _seq);

    /**
     * @brief
     * @param _seq
     */
    void setSeq(const std::string& _seq);

    /**
     * @brief
     * @return
     */
    const std::string& getQual() const;

    /**
     * @brief
     * @return
     */
    std::string&& moveQual();

    /**
     * @brief
     * @param _qual
     */
    void setQual(const std::string& _qual);

    /**
     * @brief
     * @param _flag
     * @return
     */
    bool checkFlag(uint16_t _flag) const;

    /**
     * @brief
     * @param _flag
     * @return
     */
    bool checkNFlag(uint16_t _flag) const;

    /**
     * @brief
     * @return
     */
    bool isUnmapped() const;

    /**
     * @brief
     * @return
     */
    bool isMateUnmapped() const;

    /**
     * @brief
     * @return
     */
    bool isPrimary() const;

    /**
     * @brief
     * @return
     */
    bool isSecondary() const;

    /**
     * @brief
     * @return
     */
    bool isDuplicates() const;

    /**
     * @brief
     * @return
     */
    bool isSupplementary() const;

    /**
     * @brief
     * @return
     */
    bool isPaired() const;

    /**
     * @brief
     * @return
     */
    bool isRead1() const;

    /**
     * @brief
     * @return
     */
    bool isRead2() const;

    /**
     * @brief SamRecord are correctly oriented with respect to one another,
     * i.e. that one of the mate pairs maps to the forward strand and the other maps to the reverse strand.
     * If the mates don't map in a proper pair, that may mean that both reads map to the forward or reverse strand.
     * This includes that the reads are mapped to the same chromosomes.
     * @return
     */
    bool isProperlyPaired() const;

    /**
     * @brief
     * @return
     */
    bool isPairedAndBothMapped() const;

    /**
     * @brief
     * @return
     */
    bool isReverse() const;

    /**
     * @brief
     * @param r
     * @return
     */
    bool isPairOf(SamRecord& r);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcode_sam
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_SAM_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
