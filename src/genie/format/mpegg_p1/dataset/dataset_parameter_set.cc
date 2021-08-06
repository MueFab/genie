/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/dataset_parameter_set.h"
#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

//DatasetParameterSet::DatasetParameterSet(uint8_t _group_ID, uint16_t _ID,
//                                         const core::parameter::ParameterSet&& p2_param_set)
//    : group_ID(_group_ID),
//      ID(_ID),
//      core::parameter::ParameterSet(p2_param_set){}

// ---------------------------------------------------------------------------------------------------------------------

DatasetParameterSet::DatasetParameterSet(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length,
                                         DatasetHeader& dthd) :
//    : core::parameter::ParameterSet(),
      minor_version(fhd.getMinorVersion()),
      parameters_update_flag(dthd.getParamUpdateFlag()),
      num_U_access_units(dthd.getNumAccessUnits()),
//      dataset_group_ID(),
//      dataset_ID(),
      multiple_alignment_flag(false),
      pos_40_bits_flag(DatasetHeader::Pos40Size::OFF),
      alphabet_ID(),
      U_signature_flag(0),
      U_signature_constant_length(0),
      U_signature_length(0) {

    dataset_group_ID = reader.read<uint8_t>();
    dataset_ID = reader.read<uint16_t>();

    auto _parameter_set_ID = reader.read<uint8_t>();
    auto _parent_parameter_set_ID(reader.read<uint8_t>());

    if (minor_version != "1900" && parameters_update_flag){
        multiple_alignment_flag = reader.read<bool>(1);
        pos_40_bits_flag = reader.read<bool>(1) ? DatasetHeader::Pos40Size::ON : DatasetHeader::Pos40Size::OFF;
        alphabet_ID = reader.read<uint8_t>();

        if (num_U_access_units>0){
            U_signature_flag = reader.read<bool>(1);
            if (U_signature_flag){
                U_signature_constant_length = reader.read<bool>(1);
                if (U_signature_constant_length){
                    U_signature_length = reader.read<uint8_t>();
                }
            }
        }

        reader.flush();
    }

    core::parameter::ParameterSet(_parameter_set_ID, _parent_parameter_set_ID, reader);

    UTILS_DIE_IF(!reader.isAligned() || reader.getPos() - start_pos != length,
                 "Invalid dataset parameter set length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetParameterSet::getDatasetGroupID() const { return dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::setDatasetGroupID(uint8_t _dataset_group_ID) { dataset_group_ID = _dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DatasetParameterSet::getDatasetID() const { return dataset_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::setDatasetID(uint16_t _dataset_ID) { dataset_ID = _dataset_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetParameterSet::getLength() const {
//    /// Key c(4) Length u(64)
//    uint64_t len = (4 * sizeof(char) + 8);  // gen_info
//
//    /// ID u(8)
//    len += 1;
//
//    /// ID u(16)
//    len += 2;
//
//    len += core::parameter::ParameterSet::getLength();
//
//    return len;
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    write(tmp_writer, true);
    tmp_writer.flush();
    uint64_t len = tmp_writer.getBitsWritten() / 8;

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::write(util::BitWriter& writer, bool empty_length) const {
    /// Key of KLV format
    writer.write("pars");

    /// Length of KLV format
    if (empty_length){
        writer.write(0, 64);
    } else {
        writer.write(getLength(), 64);
    }

    writer.write(dataset_group_ID, 8); /// group_ID u(8)

    writer.write(dataset_ID, 16); /// ID u(16)

    writer.write(core::parameter::ParameterSet::getID(), 8); /// parameter_set_ID u(8)

    writer.write(core::parameter::ParameterSet::getParentID(), 8); /// parent_parameter_set_ID u(8)

    if (minor_version != "1900" && parameters_update_flag){
        writer.write(multiple_alignment_flag, 1);
        writer.write(pos_40_bits_flag==DatasetHeader::Pos40Size::ON, 1);
        writer.write(alphabet_ID, 8);

        if (num_U_access_units>0){
            writer.write(U_signature_flag, 1);
            if (U_signature_flag){
                writer.write(U_signature_constant_length, 1);
                if (U_signature_constant_length){
                    writer.write(U_signature_length, 8);
                }
            }
        }

        writer.flush();
    }

    // parameter_set_ID u(8), parent_parameter_set_ID u(8), encoding parameter()
    core::parameter::ParameterSet::writeEncodingParams(writer);

    writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
