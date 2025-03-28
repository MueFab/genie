/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_CONSTANTS_H_
#define SRC_GENIE_CORE_CONSTANTS_H_

// ---------------------------------------------------------------------------------------------------------------------

#define NOMINMAX
#include <algorithm>
#include <boost/variant/variant.hpp>
#include <cmath>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "genie/util/literal.h"

// -----------------------------------------------------------------------------

namespace genie::core {

using util::operator""_u8;
using util::operator""_u16;
using util::operator""_u32;
using util::operator""_u64;

/**
 * @brief
 */
enum class AlphabetId : uint8_t {
  kAcgtn = 0,  // NOLINT
  kAcgtryswkmbdhvn = 1,
  kCount = 2
};

enum class MpegMinorVersion : uint8_t { kV1900 = 0, kV2000 = 1, kUnknown = 2 };

/**
 * @brief
 * @param v
 * @return
 */
const std::string& GetMpegVersionString(MpegMinorVersion v);

/**
 * @brief
 * @param v
 * @return
 */
MpegMinorVersion GetMpegVersion(const std::string& v);

/**
 * @brief
 */
enum class GenDesc : uint8_t {
  kPosition = 0,
  kReverseComplement = 1,
  kFlags = 2,
  kMismatchPosition = 3,
  kMismatchType = 4,
  kClips = 5,
  kUnalignedReads = 6,
  kReadLength = 7,
  kPair = 8,
  kMappingScore = 9,
  kMultiMap = 10,
  kMultiSegmentAlignment = 11,
  kRtype = 12,
  kReadGroup = 13,
  kQv = 14,
  kReadName = 15,
  kRefTransPosition = 16,
  kRefTransTransform = 17,
  kCount = 18
};

/**
 * @brief
 */
using GenSubIndex = std::pair<GenDesc, uint16_t>;

/**
 * @brief
 */
namespace gen_sub {
constexpr GenSubIndex kPositionFirst = {GenDesc::kPosition, 0_u16};
constexpr GenSubIndex kPositionAdditional = {GenDesc::kPosition, 1_u16};
constexpr GenSubIndex kReverseComplement = {GenDesc::kReverseComplement, 0_u16};
constexpr GenSubIndex kFlagsPcrDuplicate = {GenDesc::kFlags, 0_u16};
constexpr GenSubIndex kFlagsQualityFail = {GenDesc::kFlags, 1_u16};
constexpr GenSubIndex kFlagsProperPair = {GenDesc::kFlags, 2_u16};
constexpr GenSubIndex kMismatchPosTerminator = {GenDesc::kMismatchPosition,
                                                0_u16};
constexpr GenSubIndex kMismatchPosDelta = {GenDesc::kMismatchPosition, 1_u16};
constexpr GenSubIndex kMismatchType = {GenDesc::kMismatchType, 0_u16};
constexpr GenSubIndex kMismatchTypeSubstBase = {GenDesc::kMismatchType, 1_u16};
constexpr GenSubIndex kMismatchTypeInsert = {GenDesc::kMismatchType, 2_u16};
constexpr GenSubIndex kClipsRecordId = {GenDesc::kClips, 0_u16};
constexpr GenSubIndex kClipsType = {GenDesc::kClips, 1_u16};
constexpr GenSubIndex kClipsSoftClip = {GenDesc::kClips, 2_u16};
constexpr GenSubIndex kClipsHardClip = {GenDesc::kClips, 3_u16};
constexpr GenSubIndex kUnalignedReads = {GenDesc::kUnalignedReads, 0_u16};
constexpr GenSubIndex kReadLength = {GenDesc::kReadLength, 0_u16};
constexpr GenSubIndex kPairDecodingCase = {GenDesc::kPair, 0_u16};
constexpr GenSubIndex kPairSameRec = {GenDesc::kPair, 1_u16};
constexpr GenSubIndex kPairR1Split = {GenDesc::kPair, 2_u16};
constexpr GenSubIndex kPairR2Split = {GenDesc::kPair, 3_u16};
constexpr GenSubIndex kPairR1DiffSeq = {GenDesc::kPair, 4_u16};
constexpr GenSubIndex kPairR2DiffSeq = {GenDesc::kPair, 5_u16};
constexpr GenSubIndex kPairR1DiffPos = {GenDesc::kPair, 6_u16};
constexpr GenSubIndex kPairR2DiffPos = {GenDesc::kPair, 7_u16};
constexpr GenSubIndex kMappingScore = {GenDesc::kMappingScore, 0_u16};
constexpr GenSubIndex kMmapNumberAlign = {GenDesc::kMultiMap, 0_u16};
constexpr GenSubIndex kMmapRightAlignId = {GenDesc::kMultiMap, 1_u16};
constexpr GenSubIndex kMmapOtherRecFlag = {GenDesc::kMultiMap, 2_u16};
constexpr GenSubIndex kMmapRefSeq = {GenDesc::kMultiMap, 3_u16};
constexpr GenSubIndex kMmapRefPos = {GenDesc::kMultiMap, 4_u16};
constexpr GenSubIndex kMultiSegmentAlignmentCabac0 = {
    GenDesc::kMultiSegmentAlignment, 0_u16};
constexpr GenSubIndex kMultiSegmentAlignmentCabac1 = {
    GenDesc::kMultiSegmentAlignment, 1_u16};
constexpr GenSubIndex kRtype = {GenDesc::kRtype, 0_u16};
constexpr GenSubIndex kReadGroup = {GenDesc::kReadGroup, 0_u16};
constexpr GenSubIndex kQvPresent = {GenDesc::kQv, 0_u16};
constexpr GenSubIndex kQvCodebook = {GenDesc::kQv, 1_u16};
constexpr GenSubIndex kQvSteps0 = {GenDesc::kQv, 2_u16};
constexpr GenSubIndex kQvSteps1 = {GenDesc::kQv, 3_u16};
constexpr GenSubIndex kQvSteps2 = {GenDesc::kQv, 4_u16};
constexpr GenSubIndex kQvSteps3 = {GenDesc::kQv, 5_u16};
constexpr GenSubIndex kQvSteps4 = {GenDesc::kQv, 6_u16};
constexpr GenSubIndex kQvSteps5 = {GenDesc::kQv, 7_u16};
constexpr GenSubIndex kQvSteps6 = {GenDesc::kQv, 8_u16};
constexpr GenSubIndex kQvSteps7 = {GenDesc::kQv, 9_u16};
constexpr GenSubIndex kReadNameCabac0 = {GenDesc::kReadName, 0_u16};
constexpr GenSubIndex kReadNameCabac1 = {GenDesc::kReadName, 1_u16};
constexpr GenSubIndex kRefTransPosition = {GenDesc::kRefTransPosition, 0_u16};
constexpr GenSubIndex kRefTransTransform = {GenDesc::kRefTransTransform, 0_u16};
};  // namespace gen_sub

/**
 * @brief
 */
namespace gen_const {
constexpr uint8_t kMismatchPositionPersist = 0;  // NOLINT
constexpr uint8_t kMismatchPosTerminate = 1;    // NOLINT
constexpr uint8_t kFlagsPcrDuplicatePos = 0;
constexpr uint8_t kFlagsPcrDuplicateMask = 1u << kFlagsPcrDuplicatePos;
constexpr uint8_t kFlagsQualityFailPos = 1;
constexpr uint8_t kFlagsQualityFailMask = 1u << kFlagsQualityFailPos;
constexpr uint8_t kFlagsProperPairPos = 2;
constexpr uint8_t kFlagsProperPairMask = 1u << kFlagsProperPairPos;
constexpr uint8_t kMismatchTypeSubstitution = 0;
constexpr uint8_t kMismatchTypeInsertion = 1;
constexpr uint8_t kMismatchTypeDeletion = 2;
constexpr uint8_t kClipsRecordEnd = 8;
constexpr uint8_t kPairSameRecord = 0;
constexpr uint8_t kPairR1Split = 1;
constexpr uint8_t kPairR2Split = 2;
constexpr uint8_t kPairR1DiffRef = 3;
constexpr uint8_t kPairR2DiffRef = 4;
constexpr uint8_t kPairR1Unpaired = 5;
constexpr uint8_t kPairR2Unpaired = 6;
constexpr uint8_t kRtypeReference = 0;
constexpr uint8_t kRtypeClassP = 1;
constexpr uint8_t kRtypeClassN = 2;
constexpr uint8_t kRtypeClassM = 3;
constexpr uint8_t kRtypeClassI = 4;
constexpr uint8_t kRtypeClassU = 5;
constexpr uint8_t kRtypeClassHm = 6;
};  // namespace gen_const

/**
 * @brief
 */
struct GenomicSubDescriptorProperties {
  GenSubIndex id;                     //!< @brief
  std::string name;                   //!< @brief
  bool mismatch_decoding;             //!< @brief
  std::pair<int64_t, int64_t> range;  //!< @brief
};

/**
 * @brief
 */
struct GenomicDescriptorProperties {
  GenDesc id;                                            //!< @brief
  std::string name;                                      //!< @brief
  bool token_type;                                       //!< @brief
  std::vector<GenomicSubDescriptorProperties> sub_seqs;  //!< @brief

  /**
   * @brief
   * @param sub
   * @return
   */
  [[nodiscard]] const GenomicSubDescriptorProperties& GetSubSeq(
      uint8_t sub) const;
};

/**
 * @brief
 */
struct Alphabet {
  std::vector<char> lut;          //!< @brief
  std::vector<char> inverse_lut;  //!< @brief
  uint8_t base_bits;

  /**
   * @brief
   * @param c
   * @return
   */
  [[nodiscard]] bool IsIncluded(char c) const;
};

/**
 * @brief
 * @return
 */
const std::vector<GenomicDescriptorProperties>& GetDescriptors();

/**
 * @brief
 * @param desc
 * @return
 */
const GenomicDescriptorProperties& GetDescriptor(GenDesc desc);

/**
 * @brief
 * @param idx
 * @return
 */
const GenomicSubDescriptorProperties& GetSubsequence(GenSubIndex idx);

/**
 * @brief
 * @param id
 * @return
 */
const Alphabet& GetAlphabetProperties(AlphabetId id);

/**
 * @brief
 */
struct CigarFormatInfo {
  std::vector<uint8_t> lut_step_ref;    //!< @brief
  std::vector<uint8_t> lut_step_bases;  //!< @brief
  std::vector<uint8_t> lut_ignore;      //!< @brief
  bool explicit_substitution;           //!< @brief
};

/**
 * @brief
 * @return
 */
const CigarFormatInfo& GetECigarInfo();

/**
 * @brief
 * @param bits
 * @return
 */
uint8_t Bits2Bytes(uint8_t bits);

/**
 * @brief
 * @param range
 * @return
 */
uint8_t Range2Bytes(std::pair<int64_t, int64_t> range);


enum class AnnotDesc : uint8_t {
    SEQUENCEID = 1,
    STARTPOS = 2,
    ENDPOS = 3,
    STRAND = 4,
    NAME = 5,
    DESCRIPTION = 6,
    LINKNAME = 7,
    LINKID = 8,
    DEPTH = 9,
    SEQQUALITY = 10,
    MAPQUALITY = 11,
    MAPNUMQUALITY0 = 12,
    REFERENCE = 13,
    ALTERN = 14,
    GENOTYPE = 15,
    LIKELIHOOD = 16,
    FILTER = 17,
    FEATURENAME = 18,
    FEATUREID = 19,
    ONTOLOGYNAME = 20,
    ONTOLOGYID = 21,
    CONTACT = 22,
    ATTRIBUTE = 31
};

std::string AnnotDescToString(AnnotDesc desc);

enum class AlgoID : uint8_t {
    CABAC = 0,
    LZMA = 1,
    ZSTD = 2,
    BSC = 3,
    PROCRUSTES = 4,
    JBIG = 5,
    LZW = 16,
    BIN = 17,
    SPARSE = 18,
    DEL = 19,
    RLE = 20,
    SER = 21,
    UNDEFINED = 22
};

AlgoID stringToAlgoID(std::string& algostring);

enum class DataType : uint8_t {
    STRING = 0,
    CHAR = 1,
    BOOL = 2,
    INT8 = 3,
    UINT8 = 4,
    INT16 = 5,
    UINT16 = 6,
    INT32 = 7,
    UINT32 = 8,
    INT64 = 9,
    UINT64 = 10,
    FLOAT = 11,
    DOUBLE = 12
};

typedef boost::variant<std::string, char, bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t,
                       uint64_t, float, double>
    DynamicDataType;

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_CONSTANTS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
