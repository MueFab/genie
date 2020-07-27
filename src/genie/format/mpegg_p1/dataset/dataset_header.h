/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATASET_HEADER_H
#define GENIE_DATASET_HEADER_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>

#include "genie/util/bitwriter.h"
#include "fields/block_header_flags/block_header_flags.h"
#include "fields/seq_info/seq_info.h"
#include "fields/u_access_unit_info/u_access_unit_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class DatasetHeader {
   private:
    /**
     * ISO 23092-1 Section 6.5.2 table 19
     *
     * ------------------------------------------------------------------------------------------------------------- */
    uint8_t dataset_group_ID;
    uint16_t dataset_ID;
    std::string version;
    bool byte_offset_size_flag;
    bool non_overlapping_AU_range_flag;
    bool pos_40_bits_flag;

    // block_header_flag, MIT_flag, CC_mode_flag, ordered_blocks_flag, num_classes, clid[],
    // num_descriptors[], descriptor_ID[][]
    BlockHeader block_header;

    // seq_count, reference_ID, seq_ID[], seq_blocks[], tflag[], thres[]
    SequenceInfo seq_info;

    uint8_t dataset_type;

    uint8_t alphabet_ID;
    uint32_t num_U_access_units;

    //num_U_clusters, multiple_signature_base, U_signature_size, U_signature_constant_length, U_signature_length
    std::unique_ptr<UAccessUnitInfo> u_access_unit_info;

   public:
    explicit DatasetHeader(uint16_t datasetID);

    DatasetHeader(uint8_t group_ID, uint16_t ID, bool _byte_offset_size_flag, bool _non_overlapping_AU_range_flag,
                  bool _pos_40_bits_flag, uint8_t _dataset_type, uint8_t _alphabet_ID, uint32_t _num_U_access_units);

    uint64_t getLength() const;

    uint16_t getDatasetId() const;
    uint8_t getDatasetGroupId() const;

    void setDatasetGroupId(uint8_t group_ID);

    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_DATASET_HEADER_H
