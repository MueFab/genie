#include "constants.h"
#include <algorithm>

namespace genie {

constexpr GenSubIndex GenSub::POS_MAPPING_FIRST;
constexpr GenSubIndex GenSub::POS_MAPPING_ADDITIONAL;

constexpr GenSubIndex GenSub::RCOMP;

constexpr GenSubIndex GenSub::FLAGS_PCR_DUPLICATE;
constexpr GenSubIndex GenSub::FLAGS_QUALITY_FAIL;
constexpr GenSubIndex GenSub::FLAGS_PROPER_PAIR;

constexpr GenSubIndex GenSub::MMPOS_TERMINATOR;
constexpr GenSubIndex GenSub::MMPOS_POSITION;

constexpr GenSubIndex GenSub::MMTYPE_TYPE;
constexpr GenSubIndex GenSub::MMTYPE_SUBSTITUTION;
constexpr GenSubIndex GenSub::MMTYPE_INSERTION;

constexpr GenSubIndex GenSub::CLIPS_RECORD_ID;
constexpr GenSubIndex GenSub::CLIPS_TYPE;
constexpr GenSubIndex GenSub::CLIPS_SOFT_STRING;
constexpr GenSubIndex GenSub::CLIPS_HARD_LENGTH;

constexpr GenSubIndex GenSub::UREADS;

constexpr GenSubIndex GenSub::RLEN;

constexpr GenSubIndex GenSub::PAIR_DECODING_CASE;
constexpr GenSubIndex GenSub::PAIR_SAME_REC;
constexpr GenSubIndex GenSub::PAIR_R1_SPLIT;
constexpr GenSubIndex GenSub::PAIR_R2_SPLIT;
constexpr GenSubIndex GenSub::PAIR_R1_DIFF_SEQ;
constexpr GenSubIndex GenSub::PAIR_R2_DIFF_SEQ;
constexpr GenSubIndex GenSub::PAIR_R1_DIFF_POS;
constexpr GenSubIndex GenSub::PAIR_R2_DIFF_POS;

constexpr GenSubIndex GenSub::MSCORE;

constexpr GenSubIndex GenSub::MMAP_NUMBER_ALIGN;
constexpr GenSubIndex GenSub::MMAP_RIGHT_ALIGN_ID;
constexpr GenSubIndex GenSub::MMAP_OTHER_REC_FLAG;
constexpr GenSubIndex GenSub::MMAP_REF_SEQ;
constexpr GenSubIndex GenSub::MMAP_REF_POS;

constexpr GenSubIndex GenSub::MSAR_CABAC_0;
constexpr GenSubIndex GenSub::MSAR_CABAC_1;

constexpr GenSubIndex GenSub::RTYPE;

constexpr GenSubIndex GenSub::RGROUP;

constexpr GenSubIndex GenSub::QV_PRESENT;
constexpr GenSubIndex GenSub::QV_CODEBOOK;
constexpr GenSubIndex GenSub::QV_STEPS_0;
constexpr GenSubIndex GenSub::QV_STEPS_1;
constexpr GenSubIndex GenSub::QV_STEPS_2;
constexpr GenSubIndex GenSub::QV_STEPS_3;
constexpr GenSubIndex GenSub::QV_STEPS_4;
constexpr GenSubIndex GenSub::QV_STEPS_5;
constexpr GenSubIndex GenSub::QV_STEPS_6;

constexpr GenSubIndex GenSub::RNAME_CABAC_0;
constexpr GenSubIndex GenSub::RNAME_CABAC_1;

constexpr GenSubIndex GenSub::RFTP;

constexpr GenSubIndex GenSub::RFTT;

const std::vector<GenomicDescriptorProperties> &getDescriptors() {
    static const std::vector<GenomicDescriptorProperties> prop = {
        {GenDesc::POS,
         "pos",
         false,
         {{GenSub::POS_MAPPING_FIRST, "first"}, {GenSub::POS_MAPPING_ADDITIONAL, "additional"}}},

        {GenDesc::RCOMP, "rcomp", false, {{GenSub::RCOMP, "rcomp"}}},

        {GenDesc::FLAGS,
         "flags",
         false,
         {{GenSub::FLAGS_PCR_DUPLICATE, "pcr_duplicate"},
          {GenSub::FLAGS_QUALITY_FAIL, "quality_flag"},
          {GenSub::FLAGS_PROPER_PAIR, "proper_pair"}}},

        {GenDesc::MMPOS,
         "mmpos",
         false,
         {{GenSub::MMPOS_TERMINATOR, "terminator"}, {GenSub::MMPOS_POSITION, "position"}}},

        {GenDesc::MMTYPE,
         "mmtype",
         false,
         {{GenSub::MMTYPE_TYPE, "type"},
          {GenSub::MMTYPE_SUBSTITUTION, "substitution"},
          {GenSub::MMTYPE_INSERTION, "insertion"}}},

        {GenDesc::CLIPS,
         "clips",
         false,
         {{GenSub::CLIPS_RECORD_ID, "record_id"},
          {GenSub::CLIPS_TYPE, "type"},
          {GenSub::CLIPS_SOFT_STRING, "soft_string"},
          {GenSub::CLIPS_HARD_LENGTH, "hard_length"}}},

        {GenDesc::UREADS, "ureads", false, {{GenSub::UREADS, "ureads"}}},

        {GenDesc::RLEN, "rlen", false, {{GenSub::RLEN, "rlen"}}},

        {GenDesc::PAIR,
         "pair",
         false,
         {{GenSub::PAIR_DECODING_CASE, "decoding_case"},
          {GenSub::PAIR_SAME_REC, "same_rec"},
          {GenSub::PAIR_R1_SPLIT, "r1_split"},
          {GenSub::PAIR_R2_SPLIT, "r2_split"},
          {GenSub::PAIR_R1_DIFF_SEQ, "r1_diff_seq"},
          {GenSub::PAIR_R2_DIFF_SEQ, "r2_diff_seq"},
          {GenSub::PAIR_R1_DIFF_POS, "r1_diff_pos"},
          {GenSub::PAIR_R2_DIFF_POS, "r2_diff_pos"}}},

        {GenDesc::MSCORE, "mscore", false, {{GenSub::MSCORE, "mscore"}}},

        {GenDesc::MMAP,
         "mmap",
         false,
         {{GenSub::MMAP_NUMBER_ALIGN, "number_alignments"},
          {GenSub::MMAP_RIGHT_ALIGN_ID, "right_alignment_id"},
          {GenSub::MMAP_OTHER_REC_FLAG, "other_rec_flag"},
          {GenSub::MMAP_REF_SEQ, "reference_seq"},
          {GenSub::MMAP_REF_POS, "reference_pos"}}},

        {GenDesc::MSAR, "msar", true, {{GenSub::MSAR_CABAC_0, "cabac_0"}, {GenSub::MSAR_CABAC_1, "cabac_1"}}},

        {GenDesc::RTYPE, "rtype", false, {{GenSub::RTYPE, "rtype"}}},

        {GenDesc::RGROUP, "rgroup", false, {{GenSub::RGROUP, "rgroup"}}},

        {GenDesc::QV,
         "qv",
         false,
         {{GenSub::QV_PRESENT, "present"},
          {GenSub::QV_CODEBOOK, "codebook"},
          {GenSub::QV_STEPS_0, "steps_0"},
          {GenSub::QV_STEPS_1, "steps_1"},
          {GenSub::QV_STEPS_2, "steps_2"},
          {GenSub::QV_STEPS_3, "steps_3"},
          {GenSub::QV_STEPS_4, "steps_4"},
          {GenSub::QV_STEPS_5, "steps_5"},
          {GenSub::QV_STEPS_6, "steps_6"}}},

        {GenDesc::RNAME, "rname", true, {{GenSub::RNAME_CABAC_0, "cabac0"}, {GenSub::RNAME_CABAC_1, "cabac1"}}},

        {GenDesc::RFTP, "rftp", false, {{GenSub::RFTP, "rftp"}}},

        {GenDesc ::RFTT, "rftt", false, {{GenSub::RFTT, "rftt"}}}};
    return prop;
}

// ---------------------------------------------------------------------------------------------------------------------

const GenomicDescriptorProperties &getDescriptor(GenDesc desc) { return getDescriptors()[uint8_t(desc)]; }

// ---------------------------------------------------------------------------------------------------------------------

const GenomicSubDescriptorProperties &getSubsequence(GenSubIndex idx) {
    return getDescriptors()[uint8_t(idx.first)].subseqs[uint8_t(idx.second)];
}

// ---------------------------------------------------------------------------------------------------------------------

const Alphabet &getAlphabetProperties(AlphabetID id) {
    static const auto prop = []() -> std::vector<Alphabet> {
        std::vector<Alphabet> loc;
        loc.emplace_back();
        loc.back().lut = {'A', 'C', 'G', 'T', 'N'};
        loc.emplace_back();
        loc.back().lut = {'A', 'C', 'G', 'T', 'R', 'Y', 'S', 'W', 'K', 'M', 'B', 'D', 'H', 'V', 'N', '-'};
        for (auto &l : loc) {
            l.inverseLut = std::vector<char>(*std::max_element(l.lut.begin(), l.lut.end()) + 1, 0);
            for (size_t i = 0; i < l.lut.size(); ++i) {
                l.inverseLut[l.lut[i]] = i;
            }
        }
        return loc;
    }();
    return prop[uint8_t(id)];
}
}  // namespace genie