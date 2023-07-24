/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

#include "genie/core/record/data_unit/record.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace data_unit {

Record::Record() : data_unit_size(0), data_unit_type(0) {}

Record::Record(RawReference rawReference) : rawReference(rawReference), data_unit_size(0), data_unit_type(0) {}

Record::Record(ParameterSet parameterSet) : parameterSet(parameterSet), data_unit_type(1) {
    data_unit_size = this->parameterSet.getSize();
}



Record::Record(AccessUnit accessUnit) : accessUnit(accessUnit), data_unit_size(0), data_unit_type(2) {}

Record::Record(annotation_parameter_set::Record annotationParameterSet)
    : annotationParameterSet(annotationParameterSet), data_unit_size(0), data_unit_type(3) {}

Record::Record(annotation_access_unit::Record annotationAccessUnit)
    : annotationAccessUnit(annotationAccessUnit), data_unit_size(0), data_unit_type(4) {}

}  // namespace data_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
