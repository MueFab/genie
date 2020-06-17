/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset_header.h"
#include <math.h>
#include <bitset>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DatasetHeader::DatasetHeader(const uint16_t x_datasetID)
    : dataset_group_ID(0),
      dataset_ID(x_datasetID),
      version("0000"),
      byte_offset_size_flag(1),
      non_overlapping_AU_range_flag(1),
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
      alphabet_ID(170),
      num_U_access_units(0),
      num_U_clusters(0),
      multiple_signature_base(0),
      U_signature_size(0),
      U_signature_constant_length(0),
      U_signature_length(0),
      tflag(0),
      thres(0) {
    version[0] = 'a';
    version[1] = 'b';
    version[2] = 'c';
    version[3] = 'd';
}

// ---------------------------------------------------------------------------------------------------------------------

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
      version("0000"),
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

// ---------------------------------------------------------------------------------------------------------------------

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
        length += 8;                      // reference_ID
        length += (16 + 32) * seq_count;  // seq_ID + seq_blocks
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
        for (int i = 1; i < seq_count; i++) {
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

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::writeToFile(genie::util::BitWriter& bitWriter) const {
    bitWriter.write("dthd");

    bitWriter.write(this->getLength(), 64);

    bitWriter.write(dataset_group_ID, 8);
    bitWriter.write(dataset_ID, 16);
    bitWriter.write(version);  // FIXME check for too short strings

    std::string toWrite;

    toWrite += std::bitset<1>(byte_offset_size_flag).to_string();
    // bitWriter.write(byte_offset_size_flag, 1);

    toWrite += std::bitset<1>(non_overlapping_AU_range_flag).to_string();
    // bitWriter.write(non_overlapping_AU_range_flag, 1);

    toWrite += std::bitset<1>(pos_40_bits_flag).to_string();
    // bitWriter.write(pos_40_bits_flag, 1);

    toWrite += std::bitset<1>(block_header_flag).to_string();
    // bitWriter.write(block_header_flag, 1);

    if (block_header_flag) {
        toWrite += std::bitset<1>(MIT_flag).to_string();
        toWrite += std::bitset<1>(CC_mode_flag).to_string();
    } else {
        toWrite += std::bitset<1>(ordered_blocks_flag).to_string();
    }
    toWrite += std::bitset<16>(seq_count).to_string();
    if (seq_count > 0) {  // TODO
        bitWriter.write(reference_ID, 8);
        for (int seq = 0; seq < seq_count; seq++) {
            bitWriter.write(seq_ID[seq], 16);
        }
        for (int seq = 0; seq < seq_count; seq++) {
            bitWriter.write(seq_blocks[seq], 32);
        }
    }
    toWrite += std::bitset<4>(dataset_type).to_string();
    if (MIT_flag == 1) {  // TODO
        bitWriter.write(num_classes, 4);
        for (int ci = 0; ci < num_classes; ci++) {
            bitWriter.write(clid[ci], 4);
            if (!block_header_flag) {
                bitWriter.write(num_descriptors[ci], 5);
                for (int di = 0; di < num_descriptors[ci]; di++) {
                    bitWriter.write(descriptor_ID[ci][di], 7);
                }
            }
        }
    }
    toWrite += std::bitset<8>(alphabet_ID).to_string();
    toWrite += std::bitset<32>(num_U_access_units).to_string();
    if (num_U_access_units > 0) {  // TODO
        bitWriter.write(num_U_clusters, 32);
        bitWriter.write(multiple_signature_base, 31);
        if (multiple_signature_base > 0) {
            bitWriter.write(U_signature_size, 6);
        }
        bitWriter.write(U_signature_constant_length, 1);
        if (U_signature_constant_length) {
            bitWriter.write(U_signature_length, 8);
        }
    }
    if (seq_count > 0) {
        bitWriter.write(1, 1);  // tflag[0]
        bitWriter.write(thres[0], 31);
        for (int i = 1; i < seq_count; i++) {
            bitWriter.write(tflag[i], 1);
            if (tflag[i] == 1) {
                bitWriter.write(thres[i], 31);
            }
        }
    }
    /*
        if (block_header_flag) {
            bitWriter.write(MIT_flag, 1);
            bitWriter.write(CC_mode_flag, 1);
        } else {
            bitWriter.write(ordered_blocks_flag, 1);
        }
        bitWriter.write(seq_count, 16);
        if (seq_count > 0) {
            bitWriter.write(reference_ID, 8);
            for (int seq = 0; seq < seq_count; seq++) {
                bitWriter.write(seq_ID[seq], 16);
            }
            for (int seq = 0; seq < seq_count; seq++) {
                bitWriter.write(seq_blocks[seq], 32);
            }
        }
        bitWriter.write(dataset_type, 4);
        if (MIT_flag == 1) {
            bitWriter.write(num_classes, 4);
            for (int ci = 0; ci < num_classes; ci++) {
                bitWriter.write(clid[ci], 4);
                if (!block_header_flag) {
                    bitWriter.write(num_descriptors[ci], 5);
                    for (int di = 0; di < num_descriptors[ci]; di++) {
                        bitWriter.write(descriptor_ID[ci][di], 7);
                    }
                }
            }
        }
        bitWriter.write(alphabet_ID, 8);
        bitWriter.write(num_U_access_units, 32);
        if (num_U_access_units > 0) {
            bitWriter.write(num_U_clusters, 32);
            bitWriter.write(multiple_signature_base, 31);
            if (multiple_signature_base > 0) {
                bitWriter.write(U_signature_size, 6);
            }
            bitWriter.write(U_signature_constant_length, 1);
            if (U_signature_constant_length) {
                bitWriter.write(U_signature_length, 8);
            }
        }
        if (seq_count > 0) {
            bitWriter.write(1, 1);  // tflag[0]
            bitWriter.write(thres[0], 31);
            for (int i = 1; i < seq_count; i++) {
                bitWriter.write(tflag[i], 1);
                if (tflag[i] == 1) {
                    bitWriter.write(thres[i], 31);
                }
            }
        }
        */
    while (toWrite.size() % 8) {
        toWrite += "0";
    }

    //    fprintf(stdout, "toWrite: %s %lu\n", toWrite.c_str(), toWrite.size());
    uint8_t bytes = toWrite.size() / 8;
    for (uint8_t i = 0; i < bytes; i++) {
        uint8_t out = 0;
        for (uint8_t j = 0; j < 8; j++) {
            if (toWrite[7 - j] == '1') out += pow(2, j);

            //            fprintf(stdout, "%u %u: %u\n", i, j, out);
            //            fprintf(stdout, "char: %c\n", toWrite[7-j]);
            //            fprintf(stdout, "toWrite: %s %lu\n", toWrite.c_str(), toWrite.size());
        }
        bitWriter.write(out, 8);
        toWrite = toWrite.substr(8, toWrite.size());
    }

    // TODO
    bitWriter.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::setDatasetGroupId(uint8_t datasetGroupId) { dataset_group_ID = datasetGroupId; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DatasetHeader::getDatasetId() const { return dataset_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetHeader::getDatasetGroupId() const { return dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------