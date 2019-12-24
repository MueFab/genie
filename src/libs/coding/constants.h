#ifndef GENIE_CONSTANTS_H
#define GENIE_CONSTANTS_H

#include <cstdint>
#include <string>
#include <vector>

/**
             * ISO 23092-2 Section 5.1 table 34
             */
enum class AlphabetID : uint8_t {
    ACGTN = 0,            //!< Line 1
    ACGTRYSWKMBDHVN_ = 1  //!< Line 2
};

/**
     *
     */
enum class GenomicDescriptor : uint8_t {
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

enum class GenomicSubsequence : uint8_t {
    POS_MAPPING_FIRST = 0,
    POS_MAPPING_ADDITIONAL = 1,

    RCOMP = 0,

    FLAGS_PCR_DUPLICATE = 0,
    FLAGS_QUALITY_FAIL = 1,
    FLAGS_PROPER_PAIR = 2,

    MMPOS_TERMINATOR = 0,
    MMPOS_POSITION = 1,

    MMTYPE_TYPE = 0,
    MMTYPE_SUBSTITUTION = 1,
    MMTYPE_INSERTION = 2,

    CLIPS_RECORD_ID = 0,
    CLIPS_TYPE = 1,
    CLIPS_SOFT_STRING = 2,
    CLIPS_HARD_LENGTH = 3,

    UREADS = 0,

    RLEN = 0,

    PAIR_DECODING_CASE = 0,
    PAIR_SAME_REC = 1,
    PAIR_R1_SPLIT = 2,
    PAIR_R2_SPLIT = 3,
    PAIR_R1_DIFF_SEQ = 4,
    PAIR_R2_DIFF_SEQ = 5,
    PAIR_R1_DIFF_POS = 6,
    PAIR_R2_DIFF_POS = 7,

    MSCORE = 0,

    MMAP_NUMBER_ALIGN = 0,
    MMAP_RIGHT_ALIGN_ID = 1,
    MMAP_OTHER_REC_FLAG = 2,
    MMAP_REF_SEQ = 3,
    MMAP_REF_POS = 4,

    MSAR_CABAC_0 = 0,
    MSAR_CABAC_1 = 1,

    RTYPE = 0,

    RGROUP = 0,

    QV_PRESENT = 0,
    QV_CODEBOOK = 1,
    QV_STEPS_0 = 2,
    QV_STEPS_1 = 3,
    QV_STEPS_2 = 4,
    QV_STEPS_3 = 5,
    QV_STEPS_4 = 6,
    QV_STEPS_5 = 7,
    QV_STEPS_6 = 8,

    RNAME = 0,

    RFTP = 0,

    RFTT = 0
};

struct GenomicSubDescriptorProperties {
    GenomicSubsequence id;  //!<
    std::string name;       //!<
};

/**
 *
 */
struct GenomicDescriptorProperties {
    GenomicDescriptor id;  //!<
    std::string name;      //!<
    bool tokentype;
    std::vector<GenomicSubDescriptorProperties> subseqs;
    const GenomicSubDescriptorProperties& getSubSeq(GenomicSubsequence sub) const {
        return subseqs[uint8_t(sub)];
    }
};

/**
 *
 */
struct Alphabet {
    std::vector<char> lut;         //!<
    std::vector<char> inverseLut;  //!<
};

/**
 *
 * @return
 */
const std::vector<GenomicDescriptorProperties>& getDescriptors();

const GenomicDescriptorProperties& getDescriptor(GenomicDescriptor desc);
const GenomicSubDescriptorProperties& getSubsequence(GenomicDescriptor desc, GenomicSubsequence sub);

const Alphabet &getAlphabetProperties(AlphabetID id);

#endif  // GENIE_CONSTANTS_H
