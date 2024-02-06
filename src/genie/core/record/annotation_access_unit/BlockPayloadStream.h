/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKPAYLOADSTREAM_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKPAYLOADSTREAM_H_

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
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

//----------------------------------------------------------------------------------//

class BlockPayloadStream {
 private:
    genie::core::record::annotation_parameter_set::DescriptorID descriptor_ID;
    uint32_t block_payload_size;
    std::stringstream generic_payload;

 public:
    BlockPayloadStream();

    BlockPayloadStream(genie::core::record::annotation_parameter_set::DescriptorID _descriptorID,
                       uint32_t _block_payload_size, const std::stringstream& _generic_payload);

    BlockPayloadStream(const BlockPayloadStream& bp);
    BlockPayloadStream& operator=(const BlockPayloadStream& bp);

    std::stringstream& getPayload();

    void write(util::BitWriter& writer);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKPAYLOADSTREAM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
