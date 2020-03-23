/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset_header.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

DatasetHeader::DatasetHeader(const uint16_t x_datasetID)
    : dataset_group_ID(0),
      dataset_ID(x_datasetID),
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
      thres(0){}

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

uint64_t DatasetHeader::getLength() const {
    // length is first calculated in bits
    uint64_t length = 12 * 8;   // gen_info
    length += (1 + 2 + 4) * 8;  // dataset_group_ID, dataset_ID, version
    length += 4;
    if (block_header_flag) {
        length += 2;
    } else {
        length += 1;
    }
    length += 16;
    if (seq_count > 0) {
        length += 8;               // reference_ID
        length += 16 * seq_count;  // seq_ID
    }
    length += 4;  // dataset_type
    if (MIT_flag == 1) {
        length += 4;                // num_classes
        length += 4 * num_classes;  // clid
        if (!block_header_flag) {
            length += 5 * num_classes;                           // num_descriptors
            length += 7 * num_classes * num_descriptors.size();  // descriptor_ID
        }
    }
    length += 8 + 32;  // alphabet_ID + num_U_access_units
    if (num_U_access_units > 0) {
        length += 32 + 31;  // num_U_clusters + multiple_signature_base
        if (multiple_signature_base > 0) {
            length += 6;  // U_signature_size
        }
        length += 1;  // U_signature_constant_length
        if (U_signature_constant_length) {
            length += 8;  // U_signature_length
        }
    }
    if (seq_count > 0) {
        length += 1 + 31;  // tflag[0] + thres[0]
        for (int i = 1; i < seq_count; ++i) {
            length += 1;  // tflag[i]
            if (tflag[i] == 1) {
                length += 31;  // thres[i]
            }
        }
    }

    while (length % 8)  // byte_aligned
    {
        length++;
    }

    length /= 8;  // byte conversion
    return length;
}

void DatasetHeader::writeToFile(genie::util::BitWriter& bitWriter) const {
    bitWriter.write("dthd");

    bitWriter.write(this->getLength(), 64);

    // TODO
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie