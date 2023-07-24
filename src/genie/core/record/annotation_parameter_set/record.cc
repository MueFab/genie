/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/annotation_parameter_set/record.h"
#include <algorithm>
#include <string>
#include <utility>
#include "genie/core/arrayType.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

Record::Record()
    : parameter_set_ID(0),
      AT_ID(0),
      AT_alphabet_ID(0),
      AT_coord_size(0),
      AT_pos_40_bits_flag(false),
      n_aux_attribute_groups(0),
      tile_configuration{},
      annotation_encoding_parameters{} {}

Record::Record(uint8_t parameter_set_ID, uint8_t AT_ID, uint8_t AT_alphabet_ID, uint8_t AT_coord_size,
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

void Record::read(util::BitReader& reader) {
    parameter_set_ID = static_cast<uint8_t>(reader.read_b(8));
    AT_ID = static_cast<uint8_t>(reader.read_b(8));
    AT_alphabet_ID = static_cast<uint8_t>(reader.read_b(8));
    reader.read_b(2);
    AT_coord_size = static_cast<uint8_t>(reader.read_b(2));
    AT_pos_40_bits_flag = static_cast<bool>(reader.read_b(1));
    n_aux_attribute_groups = static_cast<uint8_t>(reader.read_b(3));
    tile_configuration.resize(static_cast<size_t>(n_aux_attribute_groups) + 1);
    for (auto i = 0; i <= n_aux_attribute_groups; ++i) tile_configuration[i].read(reader, AT_coord_size);
    annotation_encoding_parameters.read(reader);
}

void Record::write(Writer& writer) const {
    writer.write(parameter_set_ID, 8);
    writer.write(AT_ID, 8);
    writer.write(AT_alphabet_ID, 8);
    writer.write_reserved(2);
    writer.write(AT_coord_size, 2);
    writer.write(AT_pos_40_bits_flag, 1);
    writer.write(n_aux_attribute_groups, 3);
    for (auto tileConfiguration : tile_configuration) tileConfiguration.write(writer);
    annotation_encoding_parameters.write(writer);
}

size_t Record::getSize() const {
    core::Writer writesize;
    return getSize(writesize);
}

size_t Record::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.getBitsWritten();
}


}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
