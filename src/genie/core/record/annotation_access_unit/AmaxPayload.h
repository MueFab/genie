/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_AMAXPAYLOAD_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_AMAXPAYLOAD_H_

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
#include "genie/genotype/genotype_parameters.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

class AmaxPayload {
 private:
    uint32_t nelems;
    uint8_t nbits_per_elem;
    uint8_t nelements;
    std::vector<bool> is_one_flag;
    std::vector<uint32_t> amax_elements;

 public:
    void read(genie::util::BitReader& reader) { reader.read_b(1); }
};


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_AMAXPAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
