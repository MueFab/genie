/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <cassert>

#include <algorithm>
#include <string>
#include <utility>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

#include "genie/core/record/annotation_parameter_set/TileConfiguration.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

TileConfiguration::TileConfiguration()
    : AT_coord_size(0),
      AG_class(0),
      attribute_contiguity(false),
      two_dimensional(false),
      column_major_tile_order(false),
      symmetry_mode(0),
      symmetry_minor_diagonal(false),
      attribute_dependent_tiles(false),
      default_tile_structure{},
      n_add_tile_structures(0),
      n_attributes{},
      attribute_ID{},
      n_descriptors{},
      descriptor_ID{},
      additional_tile_structure{} {}

TileConfiguration::TileConfiguration(uint8_t AT_coord_size)
    : AT_coord_size(AT_coord_size),
      AG_class(0),
      attribute_contiguity(false),
      two_dimensional(false),
      column_major_tile_order(false),
      symmetry_mode(0),
      symmetry_minor_diagonal(false),
      attribute_dependent_tiles(false),
      default_tile_structure{AT_coord_size, two_dimensional},
      n_add_tile_structures(0),
      n_attributes{},
      attribute_ID{},
      n_descriptors{},
      descriptor_ID{},
      additional_tile_structure{} {}

TileConfiguration::TileConfiguration(
    uint8_t AT_coord_size, uint8_t AG_class, bool attribute_contiguity, bool two_dimensional,
    bool column_major_tile_order, uint8_t symmetry_mode, bool symmetry_minor_diagonal, bool attribute_dependent_tiles,
    TileStructure default_tile_structure, uint16_t n_add_tile_structures, std::vector<uint16_t> n_attributes,
    std::vector<std::vector<uint16_t>> attribute_ID, std::vector<uint8_t> n_descriptors,
    std::vector<std::vector<uint8_t>> descriptor_ID, std::vector<TileStructure> additional_tile_structure)
    : AT_coord_size(AT_coord_size),
      AG_class(AG_class),
      attribute_contiguity(attribute_contiguity),
      two_dimensional(two_dimensional),
      column_major_tile_order(column_major_tile_order),
      symmetry_mode(symmetry_mode),
      symmetry_minor_diagonal(symmetry_minor_diagonal),
      attribute_dependent_tiles(attribute_dependent_tiles),
      default_tile_structure(default_tile_structure),
      n_add_tile_structures(n_add_tile_structures),
      n_attributes(n_attributes),
      attribute_ID(attribute_ID),
      n_descriptors(n_descriptors),
      descriptor_ID(descriptor_ID),
      additional_tile_structure(additional_tile_structure) {
    if (!two_dimensional) {
        column_major_tile_order = false;
        symmetry_mode = 0;
        symmetry_minor_diagonal = false;
    }
}

TileConfiguration::TileConfiguration(util::BitReader& reader, uint8_t AT_coord_size) : AT_coord_size(AT_coord_size) {
    read(reader);
}

void TileConfiguration::read(util::BitReader& reader, uint8_t ATCoordSize) {
    AT_coord_size = ATCoordSize;
    read(reader);
}

void TileConfiguration::read(util::BitReader& reader) {
    AG_class = static_cast<uint8_t>(reader.read_b(3));
    attribute_contiguity = static_cast<uint8_t>(reader.read_b(1));
    two_dimensional = static_cast<uint8_t>(reader.read_b(1));
    if (two_dimensional) {
        reader.read_b(6);
        column_major_tile_order = static_cast<uint8_t>(reader.read_b(1));
        symmetry_mode = static_cast<uint8_t>(reader.read_b(3));
        symmetry_minor_diagonal = static_cast<uint8_t>(reader.read_b(1));
    } else {
        reader.read_b(3);
    }
    attribute_dependent_tiles = static_cast<bool>(reader.read_b(1));
    default_tile_structure.read(reader, AT_coord_size, two_dimensional);
    if (attribute_dependent_tiles) {
        n_add_tile_structures = static_cast<uint16_t>(reader.read_b(16));
        n_attributes.resize(n_add_tile_structures);
        attribute_ID.resize(n_add_tile_structures);
        n_descriptors.resize(n_add_tile_structures);
        descriptor_ID.resize(n_add_tile_structures);
        additional_tile_structure.resize(n_add_tile_structures);
        for (auto i = 0; i < n_add_tile_structures; ++i) {
            n_attributes[i] = static_cast<uint16_t>(reader.read_b(16));
            attribute_ID[i].resize(n_attributes[i]);
            for (auto j = 0; j < n_attributes[i]; ++j) {
                attribute_ID[i][j] = static_cast<uint16_t>(reader.read_b(16));
            }
            n_descriptors[i] = static_cast<uint8_t>(reader.read_b(7));
            descriptor_ID[i].resize(n_descriptors[i]);
            for (auto j = 0; j < n_descriptors[i]; ++j) {
                descriptor_ID[i][j] = static_cast<uint8_t>(reader.read_b(7));
            }
            additional_tile_structure[i].read(reader);
        }
    }
}

void TileConfiguration::write(std::ostream& outputfile, bool skipEmbeddedRecord) const {
    outputfile << std::to_string(AG_class) << ",";
    outputfile << std::to_string(attribute_contiguity) << ",";
    outputfile << std::to_string(two_dimensional) << ",";
    if (two_dimensional) {
        outputfile << std::to_string(column_major_tile_order) << ",";
        outputfile << std::to_string(symmetry_mode) << ",";
        outputfile << std::to_string(symmetry_minor_diagonal) << ",";
    }
    outputfile << std::to_string(attribute_dependent_tiles) << ",";
    default_tile_structure.write(outputfile, skipEmbeddedRecord);
    if (attribute_dependent_tiles) {
        outputfile << std::to_string(n_add_tile_structures) << ",";
        for (auto i = 0; i < n_add_tile_structures; ++i) {
            outputfile << std::to_string(n_attributes[i]) << ",";
            for (auto ID : attribute_ID[i]) outputfile << std::to_string(ID) << ",";
            outputfile << std::to_string(n_descriptors[i]) << ",";
            for (auto ID : descriptor_ID[i]) outputfile << std::to_string(ID) << ",";
            additional_tile_structure[i].write(outputfile, skipEmbeddedRecord);
        }
    }
}

void TileConfiguration::write(std::ostream& outputfile) const { write(outputfile, false); }

void TileConfiguration::write(util::BitWriter& writer, bool skipEmbeddedRecord) const {
    writer.write(AG_class, 3);
    writer.write(attribute_contiguity, 1);
    writer.write(two_dimensional, 1);
    if (two_dimensional) {
        writer.write(0, 6);
        writer.write(column_major_tile_order, 1);
        writer.write(symmetry_mode, 3);
        writer.write(symmetry_minor_diagonal, 2);
    } else {
        writer.write(0, 3);
    }
    writer.write(attribute_dependent_tiles, 1);
    default_tile_structure.write(writer, skipEmbeddedRecord);
    if (attribute_dependent_tiles) {
        writer.write(n_add_tile_structures, 16);
        for (auto i = 0; i < n_add_tile_structures; ++i) {
            writer.write(n_attributes[i], 16);
            for (auto ID : attribute_ID[i]) writer.write(ID, 16);
            writer.write(n_descriptors[i], 7);
            for (auto ID : descriptor_ID[i]) writer.write(ID, 7);
            additional_tile_structure[i].write(writer, skipEmbeddedRecord);
        }
    }
}

void TileConfiguration::write(util::BitWriter& writer) const { write(writer, false); }

void TileConfiguration::write(core::Writer& writer) const {
    writer.write(AG_class, 3);
    writer.write(attribute_contiguity, 1);
    writer.write(two_dimensional, 1);
    if (two_dimensional) {
        writer.write_reserved(6);
        writer.write(column_major_tile_order, 1);
        writer.write(symmetry_mode, 3);
        writer.write(symmetry_minor_diagonal, 2);
    } else {
        writer.write_reserved(3);
    }
    writer.write(attribute_dependent_tiles, 1);
    default_tile_structure.write(writer);
    if (attribute_dependent_tiles) {
        writer.write(n_add_tile_structures, 16);
        for (auto i = 0; i < n_add_tile_structures; ++i) {
            writer.write(n_attributes[i], 16);
            for (auto ID : attribute_ID[i]) writer.write(ID, 16);
            writer.write(n_descriptors[i], 7);
            for (auto ID : descriptor_ID[i]) writer.write(ID, 7);
            additional_tile_structure[i].write(writer);
        }
    }
}

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
