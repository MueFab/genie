/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_DATAUNIT_H_
#define SRC_GENIE_CORE_RECORD_DATAUNIT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/AnnotationParameterSet/Record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace data_unit {

class RawReference {};
class ParameterSet {};
class AccessUnit {};
class AnnotationAccessUnit {};

/**
 *  @brief
 */
class Record {
 private:
    uint8_t data_unit_type;
    uint64_t data_unit_size;
    RawReference raw_reference;
    ParameterSet parameter_set;
    AccessUnit access_unit;
    genie::core::record::AnnotationParameterSet::Record annotation_Parameter_set;
    AnnotationAccessUnit annotation_access_unit;

 public:
    /**
     * @brief
     */
    Record();


};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace data_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_DATAUNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
