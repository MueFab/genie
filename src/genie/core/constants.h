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

enum class AlphabetID : uint8_t { ACGTN = 0, ACGTRYSWKMBDHVN_ = 1 };

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

using GenSubIndex = std::pair<GenDesc, uint8_t>;
struct GenSub {
    static constexpr GenSubIndex POS_MAPPING_FIRST = {GenDesc::POS, 0};
    static constexpr GenSubIndex POS_MAPPING_ADDITIONAL = {GenDesc::POS, 1};

    static constexpr GenSubIndex RCOMP = {GenDesc::RCOMP, 0};

    static constexpr GenSubIndex FLAGS_PCR_DUPLICATE = {GenDesc::FLAGS, 0};
    static constexpr GenSubIndex FLAGS_QUALITY_FAIL = {GenDesc::FLAGS, 1};
    static constexpr GenSubIndex FLAGS_PROPER_PAIR = {GenDesc::FLAGS, 2};

    static constexpr GenSubIndex MMPOS_TERMINATOR = {GenDesc::MMPOS, 0};
    static constexpr GenSubIndex MMPOS_POSITION = {GenDesc::MMPOS, 1};

    static constexpr GenSubIndex MMTYPE_TYPE = {GenDesc::MMTYPE, 0};
    static constexpr GenSubIndex MMTYPE_SUBSTITUTION = {GenDesc::MMTYPE, 1};
    static constexpr GenSubIndex MMTYPE_INSERTION = {GenDesc::MMTYPE, 2};

    static constexpr GenSubIndex CLIPS_RECORD_ID = {GenDesc::CLIPS, 0};
    static constexpr GenSubIndex CLIPS_TYPE = {GenDesc::CLIPS, 1};
    static constexpr GenSubIndex CLIPS_SOFT_STRING = {GenDesc::CLIPS, 2};
    static constexpr GenSubIndex CLIPS_HARD_LENGTH = {GenDesc::CLIPS, 3};

    static constexpr GenSubIndex UREADS = {GenDesc::UREADS, 0};

    static constexpr GenSubIndex RLEN = {GenDesc::RLEN, 0};

    static constexpr GenSubIndex PAIR_DECODING_CASE = {GenDesc::PAIR, 0};
    static constexpr GenSubIndex PAIR_SAME_REC = {GenDesc::PAIR, 1};
    static constexpr GenSubIndex PAIR_R1_SPLIT = {GenDesc::PAIR, 2};
    static constexpr GenSubIndex PAIR_R2_SPLIT = {GenDesc::PAIR, 3};
    static constexpr GenSubIndex PAIR_R1_DIFF_SEQ = {GenDesc::PAIR, 4};
    static constexpr GenSubIndex PAIR_R2_DIFF_SEQ = {GenDesc::PAIR, 5};
    static constexpr GenSubIndex PAIR_R1_DIFF_POS = {GenDesc::PAIR, 6};
    static constexpr GenSubIndex PAIR_R2_DIFF_POS = {GenDesc::PAIR, 7};

    static constexpr GenSubIndex MSCORE = {GenDesc::MSCORE, 0};

    static constexpr GenSubIndex MMAP_NUMBER_ALIGN = {GenDesc::MMAP, 0};
    static constexpr GenSubIndex MMAP_RIGHT_ALIGN_ID = {GenDesc::MMAP, 1};
    static constexpr GenSubIndex MMAP_OTHER_REC_FLAG = {GenDesc::MMAP, 2};
    static constexpr GenSubIndex MMAP_REF_SEQ = {GenDesc::MMAP, 3};
    static constexpr GenSubIndex MMAP_REF_POS = {GenDesc::MMAP, 4};

    static constexpr GenSubIndex MSAR_CABAC_0 = {GenDesc::MSAR, 0};
    static constexpr GenSubIndex MSAR_CABAC_1 = {GenDesc::MSAR, 1};

    static constexpr GenSubIndex RTYPE = {GenDesc::RTYPE, 0};

    static constexpr GenSubIndex RGROUP = {GenDesc::RGROUP, 0};

    static constexpr GenSubIndex QV_PRESENT = {GenDesc::QV, 0};
    static constexpr GenSubIndex QV_CODEBOOK = {GenDesc::QV, 1};
    static constexpr GenSubIndex QV_STEPS_0 = {GenDesc::QV, 2};
    static constexpr GenSubIndex QV_STEPS_1 = {GenDesc::QV, 3};
    static constexpr GenSubIndex QV_STEPS_2 = {GenDesc::QV, 4};
    static constexpr GenSubIndex QV_STEPS_3 = {GenDesc::QV, 5};
    static constexpr GenSubIndex QV_STEPS_4 = {GenDesc::QV, 6};
    static constexpr GenSubIndex QV_STEPS_5 = {GenDesc::QV, 7};
    static constexpr GenSubIndex QV_STEPS_6 = {GenDesc::QV, 8};

    static constexpr GenSubIndex RNAME_CABAC_0 = {GenDesc::RNAME, 0};
    static constexpr GenSubIndex RNAME_CABAC_1 = {GenDesc::RNAME, 1};

    static constexpr GenSubIndex RFTP = {GenDesc::RFTP, 0};

    static constexpr GenSubIndex RFTT = {GenDesc::RFTT, 0};
};

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

struct GenomicSubDescriptorProperties {
    GenSubIndex id;
    std::string name;
};

struct GenomicDescriptorProperties {
    GenDesc id;
    std::string name;
    bool tokentype;
    std::vector<GenomicSubDescriptorProperties> subseqs;
    const GenomicSubDescriptorProperties& getSubSeq(uint8_t sub) const;
};

struct Alphabet {
    std::vector<char> lut;
    std::vector<char> inverseLut;

    bool isIncluded(char c) const;
};

const std::vector<GenomicDescriptorProperties>& getDescriptors();

const GenomicDescriptorProperties& getDescriptor(GenDesc desc);
const GenomicSubDescriptorProperties& getSubsequence(GenSubIndex idx);

const Alphabet& getAlphabetProperties(AlphabetID id);

struct CigarFormatInfo {
    std::vector<uint8_t> lut_step_ref;
    std::vector<uint8_t> lut_step_bases;
    std::vector<uint8_t> lut_ignore;
    bool explicitSubstitution;
};

const CigarFormatInfo& getECigarInfo();

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONSTANTS_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------