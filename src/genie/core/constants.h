/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CONSTANTS_H
#define GENIE_CONSTANTS_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
enum class AlphabetID : uint8_t { ACGTN = 0, ACGTRYSWKMBDHVN_ = 1 };

/**
 *
 */
enum class GenDesc : uint8_t {
    POS = 0,
    RCOMP = 1,
    FLAGS = 2,
    MMPOS = 3,
    MMTYPE = 4,
    CLIPS = 5,
    UREADS = 6,
    RLEN = 7,
    PAIR = 8,
    MSCORE = 9,
    MMAP = 10,
    MSAR = 11,
    RTYPE = 12,
    RGROUP = 13,
    QV = 14,
    RNAME = 15,
    RFTP = 16,
    RFTT = 17
};

/**
 *
 */
using GenSubIndex = std::pair<GenDesc, uint16_t>;

/**
 *
 */
struct GenSub {
    static const GenSubIndex POS_MAPPING_FIRST;
    static const GenSubIndex POS_MAPPING_ADDITIONAL;

    static const GenSubIndex RCOMP;

    static const GenSubIndex FLAGS_PCR_DUPLICATE;
    static const GenSubIndex FLAGS_QUALITY_FAIL;
    static const GenSubIndex FLAGS_PROPER_PAIR;

    static const GenSubIndex MMPOS_TERMINATOR;
    static const GenSubIndex MMPOS_POSITION;

    static const GenSubIndex MMTYPE_TYPE;
    static const GenSubIndex MMTYPE_SUBSTITUTION;
    static const GenSubIndex MMTYPE_INSERTION;

    static const GenSubIndex CLIPS_RECORD_ID;
    static const GenSubIndex CLIPS_TYPE;
    static const GenSubIndex CLIPS_SOFT_STRING;
    static const GenSubIndex CLIPS_HARD_LENGTH;

    static const GenSubIndex UREADS;

    static const GenSubIndex RLEN;

    static const GenSubIndex PAIR_DECODING_CASE;
    static const GenSubIndex PAIR_SAME_REC;
    static const GenSubIndex PAIR_R1_SPLIT;
    static const GenSubIndex PAIR_R2_SPLIT;
    static const GenSubIndex PAIR_R1_DIFF_SEQ;
    static const GenSubIndex PAIR_R2_DIFF_SEQ;
    static const GenSubIndex PAIR_R1_DIFF_POS;
    static const GenSubIndex PAIR_R2_DIFF_POS;

    static const GenSubIndex MSCORE;

    static const GenSubIndex MMAP_NUMBER_ALIGN;
    static const GenSubIndex MMAP_RIGHT_ALIGN_ID;
    static const GenSubIndex MMAP_OTHER_REC_FLAG;
    static const GenSubIndex MMAP_REF_SEQ;
    static const GenSubIndex MMAP_REF_POS;

    static const GenSubIndex MSAR_CABAC_0;
    static const GenSubIndex MSAR_CABAC_1;

    static const GenSubIndex RTYPE;

    static const GenSubIndex RGROUP;

    static const GenSubIndex QV_PRESENT;
    static const GenSubIndex QV_CODEBOOK;
    static const GenSubIndex QV_STEPS_0;
    static const GenSubIndex QV_STEPS_1;
    static const GenSubIndex QV_STEPS_2;
    static const GenSubIndex QV_STEPS_3;
    static const GenSubIndex QV_STEPS_4;
    static const GenSubIndex QV_STEPS_5;
    static const GenSubIndex QV_STEPS_6;

    static const GenSubIndex RNAME_CABAC_0;
    static const GenSubIndex RNAME_CABAC_1;

    static const GenSubIndex RFTP;

    static const GenSubIndex RFTT;
};

/**
 *
 */
struct GenConst {
    static constexpr uint8_t MMPOS_PERSIST = 0;
    static constexpr uint8_t MMPOS_TERMINATE = 1;
    static constexpr uint8_t FLAGS_PCR_DUPLICATE_POS = 0;
    static constexpr uint8_t FLAGS_PCR_DUPLICATE_MASK = 1u << FLAGS_PCR_DUPLICATE_POS;
    static constexpr uint8_t FLAGS_QUALITY_FAIL_POS = 1;
    static constexpr uint8_t FLAGS_QUALITY_FAIL_MASK = 1u << FLAGS_QUALITY_FAIL_POS;
    static constexpr uint8_t FLAGS_PROPER_PAIR_POS = 2;
    static constexpr uint8_t FLAGS_PROPER_PAIR_MASK = 1u << FLAGS_PROPER_PAIR_POS;
    static constexpr uint8_t MMTYPE_SUBSTITUTION = 0;
    static constexpr uint8_t MMTYPE_INSERTION = 1;
    static constexpr uint8_t MMTYPE_DELETION = 2;
    static constexpr uint8_t CLIPS_RECORD_END = 8;
    static constexpr uint8_t PAIR_SAME_RECORD = 0;
    static constexpr uint8_t PAIR_R1_SPLIT = 1;
    static constexpr uint8_t PAIR_R2_SPLIT = 2;
    static constexpr uint8_t PAIR_R1_DIFF_REF = 3;
    static constexpr uint8_t PAIR_R2_DIFF_REF = 4;
    static constexpr uint8_t RTYPE_REFERENCE = 0;
    static constexpr uint8_t RTYPE_CLASS_P = 1;
    static constexpr uint8_t RTYPE_CLASS_N = 2;
    static constexpr uint8_t RTYPE_CLASS_M = 3;
    static constexpr uint8_t RTYPE_CLASS_I = 4;
    static constexpr uint8_t RTYPE_CLASS_U = 5;
    static constexpr uint8_t RTYPE_CLASS_HM = 6;
};

/**
 *
 */
struct GenomicSubDescriptorProperties {
    GenSubIndex id;         //!<
    std::string name;       //!<
    bool mismatchDecoding;  //!<
};

/**
 *
 */
struct GenomicDescriptorProperties {
    GenDesc id;                                           //!<
    std::string name;                                     //!<
    bool tokentype;                                       //!<
    std::vector<GenomicSubDescriptorProperties> subseqs;  //!<

    /**
     *
     * @param sub
     * @return
     */
    const GenomicSubDescriptorProperties& getSubSeq(uint8_t sub) const;
};

/**
 *
 */
struct Alphabet {
    std::vector<char> lut;         //!<
    std::vector<char> inverseLut;  //!<

    /**
     *
     * @param c
     * @return
     */
    bool isIncluded(char c) const;
};

/**
 *
 * @return
 */
const std::vector<GenomicDescriptorProperties>& getDescriptors();

/**
 *
 * @param desc
 * @return
 */
const GenomicDescriptorProperties& getDescriptor(GenDesc desc);

/**
 *
 * @param idx
 * @return
 */
const GenomicSubDescriptorProperties& getSubsequence(GenSubIndex idx);

/**
 *
 * @param id
 * @return
 */
const Alphabet& getAlphabetProperties(AlphabetID id);

/**
 *
 */
struct CigarFormatInfo {
    std::vector<uint8_t> lut_step_ref;    //!<
    std::vector<uint8_t> lut_step_bases;  //!<
    std::vector<uint8_t> lut_ignore;      //!<
    bool explicitSubstitution;            //!<
};

/**
 *
 * @return
 */
const CigarFormatInfo& getECigarInfo();

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONSTANTS_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
