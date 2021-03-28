/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset_parameter_set.h"
#include <genie/util/runtime-exception.h>
#include <genie/format/mpegg_p1/util.h>
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

DatasetParameterSet::DatasetParameterSet(util::BitReader& bit_reader, size_t length)
    : dataset_group_ID(),
      dataset_ID(),
      parameterSet_p2() {

    std::string key = readKey(bit_reader);
    UTILS_DIE_IF(key != "pars", "DatasetParameterSet is not Found");

    size_t start_pos = bit_reader.getPos();

    // dataset_group_ID u(8)
    dataset_group_ID = bit_reader.read<uint8_t>();
    // dataset_ID u(16)
    dataset_ID = bit_reader.read<uint16_t>();

    /// parameter_set_ID, parent_parameter_set_ID and encoded_parameters()
//    parameterSet_p2 = ParameterSet(bit_reader);

    bit_reader.flush();

    UTILS_DIE_IF(bit_reader.getPos()-start_pos != length, "Invalid DatasetParameterSet length!");

}
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::setDatasetID(uint16_t datasetID) { dataset_ID = datasetID;}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DatasetParameterSet::getDatasetID() const { return dataset_ID;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetParameterSet::getDatasetGroupID() const { return dataset_group_ID;}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::setDatasetGroupID(uint8_t datasetGroupID) { dataset_group_ID = datasetGroupID;}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetParameterSet::getLength() const {

    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);   // gen_info

    // dataset_group_ID u(8)
    len += 1;

    // dataset_ID u(16)
    len += 2;

    // parameter_set_ID u(8), parent_parameter_set_ID u(8), Length of ParameterSet without parameter_set_ID
    // Reference : ParameterSet::write()
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    parameterSet_p2.preWrite(tmp_writer);
    tmp_writer.flush();

    len += tmp_writer.getBitsWritten() / 8;

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::write(util::BitWriter& bit_writer) const {

    /// Key of KLV format
    bit_writer.write("pars");

    /// Length of KLV format
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