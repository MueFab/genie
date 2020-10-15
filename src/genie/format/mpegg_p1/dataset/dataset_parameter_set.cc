/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset_parameter_set.h"
#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DatasetParameterSet::DatasetParameterSet(const core::parameter::ParameterSet&& parameterSet)
    : dataset_group_ID(),
      dataset_ID(),
      parameterSet_p2(parameterSet) {
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetParameterSet::DatasetParameterSet(uint8_t group_ID, uint16_t ID,
                                         const core::parameter::ParameterSet&& parameterSet)
    : dataset_group_ID(group_ID),
      dataset_ID(ID),
      parameterSet_p2(parameterSet) {}

// ---------------------------------------------------------------------------------------------------------------------

DatasetParameterSet::DatasetParameterSet(uint16_t ID,
                                         const core::parameter::ParameterSet&& parameterSet)
    : dataset_group_ID(),
      dataset_ID(ID),
      parameterSet_p2(parameterSet) {}

// ---------------------------------------------------------------------------------------------------------------------


uint16_t DatasetParameterSet::getDatasetID() const { return dataset_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetParameterSet::getDatasetGroupID() const { return dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::setDatasetGroupId(uint8_t datasetGroupId) { dataset_group_ID = datasetGroupId; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetParameterSet::getLength() const {
    // key (4), Length (8)
    uint64_t length = 12;

    // dataset_group_ID u(8)
    length += 1;

    // dataset_ID u(16)
    length += 2;

    // parameter_set_ID u(8), parent_parameter_set_ID u(8), Length of ParameterSet without parameter_set_ID
    // Reference : ParameterSet::write()
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    parameterSet_p2.preWrite(tmp_writer);
    tmp_writer.flush();

    length += tmp_writer.getBitsWritten() / 8;

    return length;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::write(util::BitWriter& bit_writer) const {
    bit_writer.write("pars");

    // Length of KVL format
    bit_writer.write(getLength(), 64);

    // dataset_group_ID u(8)
    bit_writer.write(dataset_group_ID, 8);

    // dataset_ID u(16)
    bit_writer.write(dataset_ID, 16);

    // parameter_set_ID u(8), parent_parameter_set_ID u(8), encoding parameter()
    parameterSet_p2.write(bit_writer);

    bit_writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------