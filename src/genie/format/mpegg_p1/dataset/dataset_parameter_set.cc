/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/dataset_parameter_set.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DatasetParameterSet::DatasetParameterSet(uint8_t _group_ID, uint16_t _ID,
                                         const core::parameter::ParameterSet&& p2_param_set)
    : group_ID(_group_ID),
      ID(_ID),
      core::parameter::ParameterSet(p2_param_set){}

// ---------------------------------------------------------------------------------------------------------------------

DatasetParameterSet::DatasetParameterSet(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length):
    group_ID(reader.read<uint8_t>()),
    ID(reader.read<uint16_t>()),
    core::parameter::ParameterSet(reader) {

    UTILS_DIE_IF(!reader.isAligned() || reader.getPos() - start_pos != length,
                 "Invalid dataset parameter set length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetParameterSet::getDatasetGroupID() const { return group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::setDatasetGroupID(uint8_t _group_ID) { group_ID = _group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DatasetParameterSet::getDatasetID() const { return ID; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::setDatasetID(uint16_t _ID) { ID = _ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetParameterSet::getLength() const {
    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);  // gen_info

    /// ID u(8)
    len += 1;

    /// ID u(16)
    len += 2;

    len += core::parameter::ParameterSet::getLength();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::write(util::BitWriter& writer) const {
    /// Key of KLV format
    writer.write("pars");

    /// Length of KLV format
    writer.write(getLength(), 64);

    // ID u(8)
    writer.write(group_ID, 8);

    // ID u(16)
    writer.write(ID, 16);

    // parameter_set_ID u(8), parent_parameter_set_ID u(8), encoding parameter()
    core::parameter::ParameterSet::write(writer);

    writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
