/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset_header.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

DatasetHeader::DatasetHeader(const uint16_t datasetID) : dataset_ID(datasetID) {}

DatasetHeader::DatasetHeader(uint8_t datasetGroupId, uint16_t datasetId/*, char *version, uint8_t byteOffsetSizeFlag,
                             uint8_t nonOverlappingAuRangeFlag, uint8_t pos40BitsFlag, uint8_t blockHeaderFlag,
                             uint8_t mitFlag, uint8_t ccModeFlag, uint8_t orderedBlocksFlag, uint16_t seqCount,
                             uint8_t referenceId, const std::vector<uint16_t> &seqId,
                             const std::vector<uint32_t> &seqBlocks, uint8_t datasetType, uint8_t numClasses,
                             const std::vector<uint8_t> &clid, const std::vector<uint8_t> &numDescriptors,
                             const std::vector<std::vector<uint8_t>> &descriptorId, uint8_t alphabetId,
                             uint32_t numUAccessUnits, uint32_t numUClusters, uint32_t multipleSignatureBase,
                             uint8_t uSignatureSize, uint8_t uSignatureConstantLength, uint8_t uSignatureLength,
                             const std::vector<uint8_t> &tflag, const std::vector<uint32_t> &thres*/) //TODO: Needs proper init
    : dataset_group_ID(datasetGroupId),
      dataset_ID(datasetId),
      version(""),
      byte_offset_size_flag(0),
      non_overlapping_AU_range_flag(0),
      pos_40_bits_flag(0),
      block_header_flag(0),
      MIT_flag(0),
      CC_mode_flag(0),
      ordered_blocks_flag(0),
      seq_count(0),
      reference_ID(0),
      seq_ID(0),
      seq_blocks(0),
      dataset_type(0),
      num_classes(0),
      clid(0),
      num_descriptors(0),
      descriptor_ID(0),
      alphabet_ID(0),
      num_U_access_units(0),
      num_U_clusters(0),
      multiple_signature_base(0),
      U_signature_size(0),
      U_signature_constant_length(0),
      U_signature_length(0),
      tflag(0),
      thres(0) {}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie