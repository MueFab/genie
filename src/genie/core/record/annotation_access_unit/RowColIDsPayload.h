/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_ROWCOLIDSPAYLOAD_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_ROWCOLIDSPAYLOAD_H_

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

class RowColIDsPayload {
 private:
    uint8_t nbits_per_elem;
    uint8_t nelements;
    std::vector<uint8_t> row_col_ids_elements;

 public:
    RowColIDsPayload();
    void read(genie::util::BitReader& reader) { reader.read_b(1); }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_ROWCOLIDSPAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
