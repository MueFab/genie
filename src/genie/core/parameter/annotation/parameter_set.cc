/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/parameter/annotation/parameter_set.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/array_type.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::parameter::annotation {

ParameterSet::ParameterSet()
    : parameter_set_ID(0),
      AT_ID(0),
      AT_alphabet_ID(core::AlphabetId::kAcgtn),
      AT_coord_size(3),
      AT_pos_40_bits_flag(false),
      n_aux_attribute_groups(0),
      tile_configuration{},
      annotation_encoding_parameters{} {}

ParameterSet::ParameterSet(uint8_t parameter_set_ID, uint8_t AT_ID, core::AlphabetId AT_alphabet_ID, uint8_t AT_coord_size,
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

void ParameterSet::read(util::BitReader& reader) {
    parameter_set_ID = static_cast<uint8_t>(reader.ReadBits(8));
    AT_ID = static_cast<uint8_t>(reader.ReadBits(8));
    AT_alphabet_ID = static_cast<core::AlphabetId>(reader.ReadBits(8));
    reader.ReadBits(2);
    AT_coord_size = static_cast<uint8_t>(reader.ReadBits(2));
    AT_pos_40_bits_flag = static_cast<bool>(reader.ReadBits(1));
    n_aux_attribute_groups = static_cast<uint8_t>(reader.ReadBits(3));
    tile_configuration.resize(static_cast<size_t>(n_aux_attribute_groups) + 1);
    for (auto idx_i = 0; idx_i <= n_aux_attribute_groups; ++idx_i) {
        tile_configuration[idx_i].read(reader, AT_coord_size);
    }
    annotation_encoding_parameters.read(reader);
}

void ParameterSet::write(util::BitWriter& writer) const {
    writer.WriteBits(parameter_set_ID, 8);
    writer.WriteBits(AT_ID, 8);
    writer.WriteBits(static_cast<uint8_t>(AT_alphabet_ID), 8);
    writer.WriteBits(0, 2);
    writer.WriteBits(AT_coord_size, 2);
    writer.WriteBits(AT_pos_40_bits_flag, 1);
    writer.WriteBits(n_aux_attribute_groups, 3);
    for (const auto& tileConfig : tile_configuration) {
        tileConfig.write(writer);
    }
    annotation_encoding_parameters.write(writer);
}

size_t ParameterSet::getSize() const {
    return 0;
}

}  // namespace genie::core::parameter::annotation
