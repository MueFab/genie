/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKPAYLOAD_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKPAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/record/annotation_parameter_set/GenotypeParameters.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"

#include "genie/core/record/annotation_access_unit/VariantsPayload.h"
#include "genie/core/record/annotation_access_unit/RowColIDsPayload.h"
#include "genie/core/record/annotation_access_unit/AmaxPayload.h"
#include "genie/core/record/annotation_access_unit/BinaryMatrixPayload.h"
#include "genie/core/record/annotation_access_unit/GenotypePayload.h"
#include "genie/core/record/annotation_access_unit/LikelihoodPayload.h"
#include "genie/core/record/annotation_access_unit/ContactMatrixBinPayload.h"
#include "genie/core/record/annotation_access_unit/ContactMatrixMatPayload.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {



class BlockPayload {
 private:
    genie::core::record::annotation_parameter_set::DescriptorID descriptor_ID;
    uint8_t num_chrs;
    GenotypePayload genotype_payload;
    LikelihoodPayload likelihood_payload;
    std::vector<ContactMatrixBinPayload> cm_bin_payload;
    ContactMatrixMatPayload cm_mat_payload;
    uint32_t block_payload_size;
    std::vector<uint8_t> generic_payload;

 public:
    BlockPayload();
    BlockPayload(util::BitReader& reader, genie::core::record::annotation_parameter_set::DescriptorID descriptorID,
                 uint8_t numChrs);

    BlockPayload(genie::core::record::annotation_parameter_set::DescriptorID descriptorID, uint32_t block_payload_size,
                 const std::vector<uint8_t>& generic_payload);

    BlockPayload(genie::core::record::annotation_parameter_set::DescriptorID descriptorID, uint8_t numChrs,
                 GenotypePayload genotype_payload, LikelihoodPayload likelihood_payload,
                 std::vector<ContactMatrixBinPayload> cm_bin_payload, ContactMatrixMatPayload cm_mat_payload,
                 uint32_t block_payload_size, const std::vector<uint8_t>& generic_payload);

    void read(util::BitReader& reader);
    void read(util::BitReader& reader, genie::core::record::annotation_parameter_set::DescriptorID descriptorID,
              uint8_t numChrs);
    void write(core::Writer& writer);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKPAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
