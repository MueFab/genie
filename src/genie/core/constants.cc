/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/constants.h"
#include <algorithm>
#include <limits>
#include <utility>
#include "genie/core/record/class-type.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

const GenSubIndex GenSub::POS_MAPPING_FIRST = std::make_pair(GenDesc::POS, (uint16_t)0);
const GenSubIndex GenSub::POS_MAPPING_ADDITIONAL = std::make_pair(GenDesc::POS, (uint16_t)1);

const GenSubIndex GenSub::RCOMP = std::make_pair(GenDesc::RCOMP, (uint16_t)0);

const GenSubIndex GenSub::FLAGS_PCR_DUPLICATE = std::make_pair(GenDesc::FLAGS, (uint16_t)0);
const GenSubIndex GenSub::FLAGS_QUALITY_FAIL = std::make_pair(GenDesc::FLAGS, (uint16_t)1);
const GenSubIndex GenSub::FLAGS_PROPER_PAIR = std::make_pair(GenDesc::FLAGS, (uint16_t)2);

const GenSubIndex GenSub::MMPOS_TERMINATOR = std::make_pair(GenDesc::MMPOS, (uint16_t)0);
const GenSubIndex GenSub::MMPOS_POSITION = std::make_pair(GenDesc::MMPOS, (uint16_t)1);

const GenSubIndex GenSub::MMTYPE_TYPE = std::make_pair(GenDesc::MMTYPE, (uint16_t)0);
const GenSubIndex GenSub::MMTYPE_SUBSTITUTION = std::make_pair(GenDesc::MMTYPE, (uint16_t)1);
const GenSubIndex GenSub::MMTYPE_INSERTION = std::make_pair(GenDesc::MMTYPE, (uint16_t)2);

const GenSubIndex GenSub::CLIPS_RECORD_ID = std::make_pair(GenDesc::CLIPS, (uint16_t)0);
const GenSubIndex GenSub::CLIPS_TYPE = std::make_pair(GenDesc::CLIPS, (uint16_t)1);
const GenSubIndex GenSub::CLIPS_SOFT_STRING = std::make_pair(GenDesc::CLIPS, (uint16_t)2);
const GenSubIndex GenSub::CLIPS_HARD_LENGTH = std::make_pair(GenDesc::CLIPS, (uint16_t)3);

const GenSubIndex GenSub::UREADS = std::make_pair(GenDesc::UREADS, (uint16_t)0);

const GenSubIndex GenSub::RLEN = std::make_pair(GenDesc::RLEN, (uint16_t)0);

const GenSubIndex GenSub::PAIR_DECODING_CASE = std::make_pair(GenDesc::PAIR, (uint16_t)0);
const GenSubIndex GenSub::PAIR_SAME_REC = std::make_pair(GenDesc::PAIR, (uint16_t)1);
const GenSubIndex GenSub::PAIR_R1_SPLIT = std::make_pair(GenDesc::PAIR, (uint16_t)2);
const GenSubIndex GenSub::PAIR_R2_SPLIT = std::make_pair(GenDesc::PAIR, (uint16_t)3);
const GenSubIndex GenSub::PAIR_R1_DIFF_SEQ = std::make_pair(GenDesc::PAIR, (uint16_t)4);
const GenSubIndex GenSub::PAIR_R2_DIFF_SEQ = std::make_pair(GenDesc::PAIR, (uint16_t)5);
const GenSubIndex GenSub::PAIR_R1_DIFF_POS = std::make_pair(GenDesc::PAIR, (uint16_t)6);
const GenSubIndex GenSub::PAIR_R2_DIFF_POS = std::make_pair(GenDesc::PAIR, (uint16_t)7);

const GenSubIndex GenSub::MSCORE = std::make_pair(GenDesc::MSCORE, (uint16_t)0);

const GenSubIndex GenSub::MMAP_NUMBER_ALIGN = std::make_pair(GenDesc::MMAP, (uint16_t)0);
const GenSubIndex GenSub::MMAP_RIGHT_ALIGN_ID = std::make_pair(GenDesc::MMAP, (uint16_t)1);
const GenSubIndex GenSub::MMAP_OTHER_REC_FLAG = std::make_pair(GenDesc::MMAP, (uint16_t)2);
const GenSubIndex GenSub::MMAP_REF_SEQ = std::make_pair(GenDesc::MMAP, (uint16_t)3);
const GenSubIndex GenSub::MMAP_REF_POS = std::make_pair(GenDesc::MMAP, (uint16_t)4);

const GenSubIndex GenSub::MSAR_CABAC_0 = std::make_pair(GenDesc::MSAR, (uint16_t)0);
const GenSubIndex GenSub::MSAR_CABAC_1 = std::make_pair(GenDesc::MSAR, (uint16_t)1);

const GenSubIndex GenSub::RTYPE = std::make_pair(GenDesc::RTYPE, (uint16_t)0);

const GenSubIndex GenSub::RGROUP = std::make_pair(GenDesc::RGROUP, (uint16_t)0);

const GenSubIndex GenSub::QV_PRESENT = std::make_pair(GenDesc::QV, (uint16_t)0);
const GenSubIndex GenSub::QV_CODEBOOK = std::make_pair(GenDesc::QV, (uint16_t)1);
const GenSubIndex GenSub::QV_STEPS_0 = std::make_pair(GenDesc::QV, (uint16_t)2);
const GenSubIndex GenSub::QV_STEPS_1 = std::make_pair(GenDesc::QV, (uint16_t)3);
const GenSubIndex GenSub::QV_STEPS_2 = std::make_pair(GenDesc::QV, (uint16_t)4);
const GenSubIndex GenSub::QV_STEPS_3 = std::make_pair(GenDesc::QV, (uint16_t)5);
const GenSubIndex GenSub::QV_STEPS_4 = std::make_pair(GenDesc::QV, (uint16_t)6);
const GenSubIndex GenSub::QV_STEPS_5 = std::make_pair(GenDesc::QV, (uint16_t)7);
const GenSubIndex GenSub::QV_STEPS_6 = std::make_pair(GenDesc::QV, (uint16_t)8);
const GenSubIndex GenSub::QV_STEPS_7 = std::make_pair(GenDesc::QV, (uint16_t)9);

const GenSubIndex GenSub::RNAME_CABAC_0 = std::make_pair(GenDesc::RNAME, (uint16_t)0);
const GenSubIndex GenSub::RNAME_CABAC_1 = std::make_pair(GenDesc::RNAME, (uint16_t)1);

const GenSubIndex GenSub::RFTP = std::make_pair(GenDesc::RFTP, (uint16_t)0);

const GenSubIndex GenSub::RFTT = std::make_pair(GenDesc::RFTT, (uint16_t)0);

constexpr uint8_t GenConst::MMPOS_PERSIST;
constexpr uint8_t GenConst::MMPOS_TERMINATE;
constexpr uint8_t GenConst::FLAGS_PCR_DUPLICATE_POS;
constexpr uint8_t GenConst::FLAGS_PCR_DUPLICATE_MASK;
constexpr uint8_t GenConst::FLAGS_QUALITY_FAIL_POS;
constexpr uint8_t GenConst::FLAGS_QUALITY_FAIL_MASK;
constexpr uint8_t GenConst::FLAGS_PROPER_PAIR_POS;
constexpr uint8_t GenConst::FLAGS_PROPER_PAIR_MASK;
constexpr uint8_t GenConst::MMTYPE_SUBSTITUTION;
constexpr uint8_t GenConst::MMTYPE_INSERTION;
constexpr uint8_t GenConst::MMTYPE_DELETION;
constexpr uint8_t GenConst::CLIPS_RECORD_END;
constexpr uint8_t GenConst::PAIR_SAME_RECORD;
constexpr uint8_t GenConst::PAIR_R1_SPLIT;
constexpr uint8_t GenConst::PAIR_R2_SPLIT;
constexpr uint8_t GenConst::PAIR_R1_DIFF_REF;
constexpr uint8_t GenConst::PAIR_R2_DIFF_REF;
constexpr uint8_t GenConst::PAIR_R1_UNPAIRED;
constexpr uint8_t GenConst::PAIR_R2_UNPAIRED;
constexpr uint8_t GenConst::RTYPE_REFERENCE;
constexpr uint8_t GenConst::RTYPE_CLASS_P;
constexpr uint8_t GenConst::RTYPE_CLASS_N;
constexpr uint8_t GenConst::RTYPE_CLASS_M;
constexpr uint8_t GenConst::RTYPE_CLASS_I;
constexpr uint8_t GenConst::RTYPE_CLASS_U;
constexpr uint8_t GenConst::RTYPE_CLASS_HM;

// ---------------------------------------------------------------------------------------------------------------------

const std::string &getMPEGVersionString(MPEGMinorVersion v) {
    static const std::string lut[] = {"1900", "2000"};
    return lut[static_cast<uint8_t>(v)];
}

// ---------------------------------------------------------------------------------------------------------------------

MPEGMinorVersion getMPEGVersion(const std::string &v) {
    for (MPEGMinorVersion ret = MPEGMinorVersion::V1900; ret < MPEGMinorVersion::UNKNOWN;
         ret = MPEGMinorVersion(static_cast<uint8_t>(ret) + 1)) {
        if (getMPEGVersionString(ret) == v) {
            return ret;
        }
    }
    return MPEGMinorVersion::UNKNOWN;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<GenomicDescriptorProperties> &getDescriptors() {
    static const std::vector<GenomicDescriptorProperties> prop = {
        {GenDesc::POS,
         "pos",
         false,
         {{GenSub::POS_MAPPING_FIRST,
           "first",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}},
          {GenSub::POS_MAPPING_ADDITIONAL,
           "additional",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}}}},

        {GenDesc::RCOMP,
         "rcomp",
         false,
         {{GenSub::RCOMP, "rcomp", false, {std::numeric_limits<bool>::min(), std::numeric_limits<bool>::max()}}}},

        {GenDesc::FLAGS,
         "flags",
         false,
         {{GenSub::FLAGS_PCR_DUPLICATE,
           "pcr_duplicate",
           false,
           {std::numeric_limits<bool>::min(), std::numeric_limits<bool>::max()}},
          {GenSub::FLAGS_QUALITY_FAIL,
           "quality_flag",
           false,
           {std::numeric_limits<bool>::min(), std::numeric_limits<bool>::max()}},
          {GenSub::FLAGS_PROPER_PAIR,
           "proper_pair",
           false,
           {std::numeric_limits<bool>::min(), std::numeric_limits<bool>::max()}}}},

        {GenDesc::MMPOS,
         "mmpos",
         false,
         {{GenSub::MMPOS_TERMINATOR,
           "terminator",
           false,
           {std::numeric_limits<bool>::min(), std::numeric_limits<bool>::max()}},
          {GenSub::MMPOS_POSITION,
           "position",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}}}},

        {GenDesc::MMTYPE,
         "mmtype",
         false,
         {{GenSub::MMTYPE_TYPE, "type", false, {std::numeric_limits<uint8_t>::min(), GenConst::MMTYPE_DELETION}},
          {GenSub::MMTYPE_SUBSTITUTION,
           "substitution",
           true,
           {0, getAlphabetProperties(AlphabetID::ACGTN).lut.size() - 1}},
          {GenSub::MMTYPE_INSERTION,
           "insertion",
           false,
           {0, getAlphabetProperties(AlphabetID::ACGTN).lut.size() - 1}}}},

        {GenDesc::CLIPS,
         "clips",
         false,
         {{GenSub::CLIPS_RECORD_ID,
           "record_id",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}},
          {GenSub::CLIPS_TYPE, "type", false, {std::numeric_limits<uint8_t>::min(), GenConst::CLIPS_RECORD_END}},
          {GenSub::CLIPS_SOFT_STRING,
           "soft_string",
           false,
           {std::numeric_limits<uint8_t>::min(), getAlphabetProperties(AlphabetID::ACGTN).lut.size()}},
          {GenSub::CLIPS_HARD_LENGTH,
           "hard_length",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}}}},

        {GenDesc::UREADS,
         "ureads",
         false,
         {{GenSub::UREADS,
           "ureads",
           false,
           {std::numeric_limits<uint8_t>::min(), getAlphabetProperties(AlphabetID::ACGTN).lut.size() - 1}}}},

        {GenDesc::RLEN,
         "rlen",
         false,
         {{GenSub::RLEN, "rlen", false, {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}}}},

        {GenDesc::PAIR,
         "pair",
         false,
         {{GenSub::PAIR_DECODING_CASE,
           "decoding_case",
           false,
           {std::numeric_limits<uint8_t>::min(), GenConst::PAIR_R2_UNPAIRED}},
          {GenSub::PAIR_SAME_REC,
           "same_rec",
           false,
           {std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max()}},
          {GenSub::PAIR_R1_SPLIT,
           "r1_split",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}},
          {GenSub::PAIR_R2_SPLIT,
           "r2_split",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}},
          {GenSub::PAIR_R1_DIFF_SEQ,
           "r1_diff_seq",
           false,
           {std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max()}},
          {GenSub::PAIR_R2_DIFF_SEQ,
           "r2_diff_seq",
           false,
           {std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max()}},
          {GenSub::PAIR_R1_DIFF_POS,
           "r1_diff_pos",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}},
          {GenSub::PAIR_R2_DIFF_POS,
           "r2_diff_pos",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}}}},

        {GenDesc::MSCORE,
         "mscore",
         false,
         {{GenSub::MSCORE,
           "mscore",
           false,
           {std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}}}},

        {GenDesc::MMAP,
         "mmap",
         false,
         {{GenSub::MMAP_NUMBER_ALIGN,
           "number_alignments",
           false,
           {std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}},
          {GenSub::MMAP_RIGHT_ALIGN_ID,
           "right_alignment_id",
           false,
           {std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}},
          {GenSub::MMAP_OTHER_REC_FLAG,
           "other_rec_flag",
           false,
           {std::numeric_limits<bool>::min(), std::numeric_limits<bool>::max()}},
          {GenSub::MMAP_REF_SEQ,
           "reference_seq",
           false,
           {std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max()}},
          {GenSub::MMAP_REF_POS,
           "reference_pos",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}}}},

        {GenDesc::MSAR,
         "msar",
         true,
         {{GenSub::MSAR_CABAC_0,
           "cabac_0",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}},
          {GenSub::MSAR_CABAC_1,
           "cabac_1",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}}}},

        {GenDesc::RTYPE,
         "rtype",
         false,
         {{GenSub::RTYPE,
           "rtype",
           false,
           {std::numeric_limits<uint8_t>::min(), static_cast<uint8_t>(genie::core::record::ClassType::COUNT)}}}},

        {GenDesc::RGROUP,
         "rgroup",
         false,
         {{GenSub::RGROUP,
           "rgroup",
           false,
           {std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}}}},

        {GenDesc::QV,
         "qv",
         false,
         {
             {GenSub::QV_PRESENT,
              "present",
              false,
              {std::numeric_limits<bool>::min(), std::numeric_limits<bool>::max()}},
             {GenSub::QV_CODEBOOK, "codebook", false, {std::numeric_limits<uint8_t>::min(), 4}},
             {GenSub::QV_STEPS_0,
              "steps_0",
              false,
              {std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}},
             {GenSub::QV_STEPS_1,
              "steps_1",
              false,
              {std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}},
             {GenSub::QV_STEPS_2,
              "steps_2",
              false,
              {std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}},
             {GenSub::QV_STEPS_3,
              "steps_3",
              false,
              {std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}},
             {GenSub::QV_STEPS_4,
              "steps_4",
              false,
              {std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}},
             {GenSub::QV_STEPS_5,
              "steps_5",
              false,
              {std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}},
             {GenSub::QV_STEPS_6,
              "steps_6",
              false,
              {std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}},
             {GenSub::QV_STEPS_7,
              "steps_7",
              false,
              {std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}},
         }},

        {GenDesc::RNAME,
         "rname",
         true,
         {{GenSub::RNAME_CABAC_0,
           "cabac0",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}},
          {GenSub::RNAME_CABAC_1,
           "cabac1",
           false,
           {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}}}},

        {GenDesc::RFTP,
         "rftp",
         false,
         {{GenSub::RFTP, "rftp", false, {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}}}},

        {GenDesc::RFTT,
         "rftt",
         false,
         {{GenSub::RFTP,
           "rftt",
           true,
           {std::numeric_limits<uint8_t>::min(), getAlphabetProperties(AlphabetID::ACGTN).lut.size() - 1}}}},
    };

    return prop;
}

// ---------------------------------------------------------------------------------------------------------------------

const GenomicDescriptorProperties &getDescriptor(GenDesc desc) { return getDescriptors()[uint8_t(desc)]; }

// ---------------------------------------------------------------------------------------------------------------------

const GenomicSubDescriptorProperties &getSubsequence(GenSubIndex idx) {
    if (getDescriptors()[uint8_t(idx.first)].tokentype) {
        return getDescriptors()[uint8_t(idx.first)].subseqs[0];
    }
    return getDescriptors()[uint8_t(idx.first)].subseqs[uint8_t(idx.second)];
}

// ---------------------------------------------------------------------------------------------------------------------

const Alphabet &getAlphabetProperties(AlphabetID id) {
    static const auto prop = []() -> std::vector<Alphabet> {
        std::vector<Alphabet> loc;
        loc.emplace_back();
        loc.back().lut = {'A', 'C', 'G', 'T', 'N'};
        loc.back().base_bits = 3;
        loc.emplace_back();
        loc.back().lut = {'A', 'C', 'G', 'T', 'R', 'Y', 'S', 'W', 'K', 'M', 'B', 'D', 'H', 'V', 'N', '-'};
        loc.back().base_bits = 5;
        for (auto &l : loc) {
            l.inverseLut = std::vector<char>(*std::max_element(l.lut.begin(), l.lut.end()) + 1, 0);
            for (size_t i = 0; i < l.lut.size(); ++i) {
                l.inverseLut[l.lut[i]] = static_cast<char>(i);
            }
        }
        return loc;
    }();
    return prop[uint8_t(id)];
}

// ---------------------------------------------------------------------------------------------------------------------

const GenomicSubDescriptorProperties &GenomicDescriptorProperties::getSubSeq(uint8_t sub) const {
    return subseqs[uint8_t(sub)];
}

// ---------------------------------------------------------------------------------------------------------------------

bool Alphabet::isIncluded(char c) const {
    for (const char &lc : lut) {
        if (lc == c) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------------------------------------------------

const CigarFormatInfo &getECigarInfo() {
    static const auto formatInfo = []() -> CigarFormatInfo {
        const auto ref_step2 = []() -> std::vector<uint8_t> {
            std::vector<uint8_t> lut(128, 0);
            lut['='] = 1;
            lut['+'] = 0;
            lut['-'] = 1;
            lut['*'] = 1;
            lut['/'] = 1;
            lut['%'] = 1;
            lut[')'] = 0;
            lut[']'] = 0;
            return lut;
        }();
        const auto seq_step2 = []() -> std::vector<uint8_t> {
            std::vector<uint8_t> lut(128, 0);
            lut['='] = 1;
            lut['+'] = 1;
            lut['-'] = 0;
            lut['%'] = 0;
            lut['/'] = 0;
            lut['*'] = 0;
            lut[')'] = 1;
            lut[']'] = 0;
            return lut;
        }();
        const auto ignore2 = []() -> std::vector<uint8_t> {
            std::vector<uint8_t> lut(128, 0);
            lut['('] = 1;
            lut['['] = 1;
            return lut;
        }();
        return {ref_step2, seq_step2, ignore2, true};
    }();
    return formatInfo;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t bits2bytes(uint8_t bits) {
    auto bytes = static_cast<uint8_t>(std::ceil(bits / 8.0f));
    bytes = static_cast<uint8_t>(std::pow(2, std::ceil(log2(bytes))));
    return bytes;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t range2bytes(std::pair<int64_t, int64_t> range) {
    auto bits = static_cast<uint8_t>(std::ceil(std::log2(std::abs(range.first) + 1)));
    bits = std::max(bits, static_cast<uint8_t>(std::ceil(std::log2(std::abs(range.second) + 1))));
    if (range.first < 0) {
        bits++;
    }
    return bits2bytes(bits);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
