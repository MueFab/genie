/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/annotation_parameter_set/record.h"

#include <sstream>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

Record::Record()
    : parameter_set_ID(0),
      AT_ID(0),
      AT_alphabet_ID(core::AlphabetId::kAcgtn),
      AT_coord_size(3),
      AT_pos_40_bits_flag(false),
      n_aux_attribute_groups(0),
      tile_configuration{},
      annotation_encoding_parameters{} {}

Record::Record(uint8_t parameter_set_ID, uint8_t AT_ID, core::AlphabetId AT_alphabet_ID, uint8_t AT_coord_size,
               bool AT_pos_40_bits_flag, uint8_t n_aux_attribute_groups,
               std::vector<TileConfiguration> tile_configuration,
               AnnotationEncodingParameters annotation_encoding_parameters)
    : parameter_set_ID(parameter_set_ID),
      AT_ID(AT_ID),
      AT_alphabet_ID(AT_alphabet_ID),
      AT_coord_size(AT_coord_size),
      AT_pos_40_bits_flag(AT_pos_40_bits_flag),
      n_aux_attribute_groups(n_aux_attribute_groups),
      tile_configuration(tile_configuration),
      annotation_encoding_parameters(annotation_encoding_parameters) {}

void Record::Read(util::BitReader& reader) {
    parameter_set_ID = static_cast<uint8_t>(reader.ReadBits(8));
    AT_ID = static_cast<uint8_t>(reader.ReadBits(8));
    AT_alphabet_ID = static_cast<core::AlphabetId>(reader.ReadBits(8));
    reader.ReadBits(2);  // reserved
    AT_coord_size = static_cast<uint8_t>(reader.ReadBits(2));
    AT_pos_40_bits_flag = static_cast<bool>(reader.ReadBits(1));
    n_aux_attribute_groups = static_cast<uint8_t>(reader.ReadBits(3));
    tile_configuration.resize(static_cast<size_t>(n_aux_attribute_groups) + 1);
    for (auto i = 0; i <= n_aux_attribute_groups; ++i) tile_configuration[i].Read(reader, AT_coord_size);
    annotation_encoding_parameters.Read(reader);
}

void Record::Write(util::BitWriter& writer) const {
    writer.WriteBits(parameter_set_ID, 8);
    writer.WriteBits(AT_ID, 8);
    writer.WriteBits(static_cast<uint8_t>(AT_alphabet_ID), 8);
    writer.WriteBits(0, 2);
    writer.WriteBits(AT_coord_size, 2);
    writer.WriteBits(AT_pos_40_bits_flag, 1);
    writer.WriteBits(n_aux_attribute_groups, 3);
    for (auto tileConfiguration : tile_configuration) tileConfiguration.Write(writer);
    annotation_encoding_parameters.Write(writer);
}

size_t Record::GetSize() const {
    std::stringstream SizeOut;
    util::BitWriter writesize(&SizeOut);
    return GetSize(writesize);
}

size_t Record::GetSize(util::BitWriter& writesize) const {
    Write(writesize);
    return writesize.GetTotalBitsWritten();
}

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
