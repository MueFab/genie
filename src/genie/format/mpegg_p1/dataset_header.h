/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATASET_HEADER_H
#define GENIE_DATASET_HEADER_H

#include <cstdint>
#include <vector>

namespace genie {
namespace format {
namespace mpegg_p1 {
class DatasetHeader {
   public:
    DatasetHeader(const uint16_t x_dataset_ID);

    void setDatasetGroupID(uint8_t x_dataset_group_ID) { dataset_group_ID = x_dataset_group_ID; }

   private:
    /**
     * ISO 23092-1 Section 6.5.2 table 19
     *
     * ------------------------------------------------------------------------------------------------------------- */
    uint8_t dataset_group_ID : 8;
    uint16_t dataset_ID : 16;
    char version[4];
    uint8_t byte_offset_size_flag : 1;
    uint8_t non_overlapping_AU_range_flag : 1;
    uint8_t pos_40_bits_flag : 1;
    uint8_t block_header_flag : 1;
    uint8_t MIT_flag : 1;
    uint8_t CC_mode_flag : 1;
    uint8_t ordered_blocks_flag : 1;
    uint16_t seq_count : 16;
    uint8_t reference_ID : 8;
    std::vector<uint16_t> seq_ID;
    std::vector<uint32_t> seq_blocks;
    uint8_t dataset_type : 4;
    uint8_t num_classes : 4;
    std::vector<uint8_t> clid;
    std::vector<uint8_t> num_descriptors;
    std::vector<std::vector<uint8_t>> descriptor_ID;
    uint8_t alphabet_ID : 8;
    uint32_t num_U_access_units : 32;
    uint32_t num_U_clusters : 32;
    uint32_t multiple_signature_base : 31;
    uint8_t U_signature_size : 6;
    uint8_t U_signature_constant_length : 1;
    uint8_t U_signature_length : 8;
    std::vector<uint8_t> tflag;
    std::vector<uint32_t> thres;
    // TODO: nesting_zero_bit:
    // while(!writer.byte_aligned()) {
    //     writer->write(0);
    // }
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_DATASET_HEADER_H
