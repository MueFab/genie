#include "constants.h"
#include <algorithm>

const std::vector<GenomicDescriptorProperties> &getDescriptors() {
    static const std::vector<GenomicDescriptorProperties> prop = {
        {GenomicDescriptor::POS,
         "pos",
         false,
         {{GenomicSubsequence::POS_MAPPING_FIRST, "first"},
          {GenomicSubsequence::POS_MAPPING_ADDITIONAL, "additional"}}},

        {GenomicDescriptor::RCOMP, "rcomp", false, {{GenomicSubsequence::RCOMP, "rcomp"}}},

        {GenomicDescriptor::FLAGS,
         "flags",
         false,
         {{GenomicSubsequence::FLAGS_PCR_DUPLICATE, "pcr_duplicate"},
          {GenomicSubsequence::FLAGS_QUALITY_FAIL, "quality_flag"},
          {GenomicSubsequence::FLAGS_PROPER_PAIR, "proper_pair"}}},

        {GenomicDescriptor::MMPOS,
         "mmpos",
         false,
         {{GenomicSubsequence::MMPOS_TERMINATOR, "terminator"}, {GenomicSubsequence::MMPOS_POSITION, "position"}}},

        {GenomicDescriptor::MMTYPE,
         "mmtype",
         false,
         {{GenomicSubsequence::MMTYPE_TYPE, "type"},
          {GenomicSubsequence::MMTYPE_SUBSTITUTION, "substitution"},
          {GenomicSubsequence::MMTYPE_SUBSTITUTION, "insertion"}}},

        {GenomicDescriptor::CLIPS,
         "clips",
         false,
         {{GenomicSubsequence::CLIPS_RECORD_ID, "record_id"},
          {GenomicSubsequence::CLIPS_TYPE, "type"},
          {GenomicSubsequence::CLIPS_SOFT_STRING, "soft_string"},
          {GenomicSubsequence::CLIPS_HARD_LENGTH, "hard_length"}}},

        {GenomicDescriptor::UREADS, "ureads", false, {{GenomicSubsequence::UREADS, "ureads"}}},

        {GenomicDescriptor::RLEN, "rlen", false, {{GenomicSubsequence::RLEN, "rlen"}}},

        {GenomicDescriptor::PAIR,
         "pair",
         false,
         {{GenomicSubsequence::PAIR_DECODING_CASE, "decoding_case"},
          {GenomicSubsequence::PAIR_SAME_REC, "same_rec"},
          {GenomicSubsequence::PAIR_R1_SPLIT, "r1_split"},
          {GenomicSubsequence::PAIR_R2_SPLIT, "r2_split"},
          {GenomicSubsequence::PAIR_R1_DIFF_SEQ, "r1_diff_seq"},
          {GenomicSubsequence::PAIR_R2_DIFF_SEQ, "r2_diff_seq"},
          {GenomicSubsequence::PAIR_R1_DIFF_POS, "r1_diff_pos"},
          {GenomicSubsequence::PAIR_R2_DIFF_POS, "r2_diff_pos"}}},

        {GenomicDescriptor::MSCORE, "mscore", false, {{GenomicSubsequence::MSCORE, "mscore"}}},

        {GenomicDescriptor::MMAP,
         "mmap",
         false,
         {{GenomicSubsequence::MMAP_NUMBER_ALIGN, "number_alignments"},
          {GenomicSubsequence::MMAP_RIGHT_ALIGN_ID, "right_alignment_id"},
          {GenomicSubsequence::MMAP_OTHER_REC_FLAG, "other_rec_flag"},
          {GenomicSubsequence::MMAP_REF_SEQ, "reference_seq"},
          {GenomicSubsequence::MMAP_REF_POS, "reference_pos"}}},

        {GenomicDescriptor::MSAR,
         "msar",
         true,
         {{GenomicSubsequence::MSAR_CABAC_0, "cabac_0"}, {GenomicSubsequence::MSAR_CABAC_1, "cabac_1"}}},

        {GenomicDescriptor::RTYPE, "rtype", false, {{GenomicSubsequence::RTYPE, "rtype"}}},

        {GenomicDescriptor::RGROUP, "rgroup", false, {{GenomicSubsequence::RGROUP, "rgroup"}}},

        {GenomicDescriptor::QV,
         "qv",
         false,
         {{GenomicSubsequence::QV_PRESENT, "present"},
          {GenomicSubsequence::QV_CODEBOOK, "codebook"},
          {GenomicSubsequence::QV_STEPS_0, "steps_0"},
          {GenomicSubsequence::QV_STEPS_1, "steps_1"},
          {GenomicSubsequence::QV_STEPS_2, "steps_2"},
          {GenomicSubsequence::QV_STEPS_3, "steps_3"},
          {GenomicSubsequence::QV_STEPS_4, "steps_4"},
          {GenomicSubsequence::QV_STEPS_5, "steps_5"},
          {GenomicSubsequence::QV_STEPS_6, "steps_6"}}},

        {GenomicDescriptor::RNAME, "rname", true, {{GenomicSubsequence::RNAME, "rname"}}},

        {GenomicDescriptor::RFTP, "rftp", false, {{GenomicSubsequence::RFTP, "rftp"}}},

        {GenomicDescriptor ::RFTT, "rftt", false, {{GenomicSubsequence::RFTT, "rftt"}}}};
    return prop;
}

// ---------------------------------------------------------------------------------------------------------------------

const GenomicDescriptorProperties &getDescriptor(GenomicDescriptor desc) { return getDescriptors()[uint8_t(desc)]; }

// ---------------------------------------------------------------------------------------------------------------------

const GenomicSubDescriptorProperties &getSubsequence(GenomicDescriptor desc, GenomicSubsequence sub) {
    return getDescriptors()[uint8_t(desc)].subseqs[uint8_t(sub)];
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