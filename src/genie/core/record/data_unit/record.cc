/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <filesystem>
#include <string>
#include <utility>
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

#include "genie/core/record/data_unit/record.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace data_unit {

Record::Record() : data_unit_type(0), data_unit_size(0) {}

Record::Record(RawReference& _rawReference) : data_unit_type(0), data_unit_size(0), rawReference(_rawReference) {}

Record::Record(ParameterSet& _parameterSet) : data_unit_type(1), parameterSet(_parameterSet) {
    data_unit_size = this->parameterSet.getSize();
}

Record::Record(AccessUnit& _accessUnit) : data_unit_type(2), data_unit_size(0), accessUnit(_accessUnit) {}

Record::Record(annotation_parameter_set::Record& _annotationParameterSet)
    : data_unit_type(3), data_unit_size(0), annotationParameterSet(_annotationParameterSet) {}

Record::Record(annotation_access_unit::Record& _annotationAccessUnit) : data_unit_type(4), data_unit_size(0) {
    annotationAccessUnit = _annotationAccessUnit;
}

void Record::write(core::Writer& writer, uint64_t writeSize) const {
    writer.write(data_unit_type, 8);
    switch (data_unit_type) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            writer.write_reserved(10);
            writer.write(writeSize, 22);
            annotationParameterSet.write(writer);
            break;
        case 4:
            writer.write_reserved(3);
            writer.write(writeSize, 29);
            annotationAccessUnit.write(writer);
            break;
        default:
            break;
    }
}

uint64_t genie::core::record::data_unit::Record::write(core::Writer& writer) const {
    writer.write(data_unit_type, 8);
    uint64_t writesize = 0;
    switch (data_unit_type) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            writer.write_reserved(10);
            writesize = (annotationParameterSet.getSize() + 40) / 8;
            writer.write(writesize, 22);
            annotationParameterSet.write(writer);
            break;
        case 4:
            writer.write_reserved(3);
            writesize = (annotationAccessUnit.getSize() + 40) / 8;
            writer.write(writesize, 29);
            annotationAccessUnit.write(writer);
            break;
        default:
            break;
    }
    return writesize;
}
}  // namespace data_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
