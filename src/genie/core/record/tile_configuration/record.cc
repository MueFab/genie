/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "record.h"
#include <algorithm>
#include <string>
#include <utility>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace tile_configuration {

uint64_t TileStructure::readVariableSize(util::BitReader& reader, uint8_t ATCoordSize) {
    switch (ATCoordSize) {
        case 0:
            return reader.readBypassBE<uint8_t>();
        case 1:
            return reader.readBypassBE<uint16_t>();
        case 2:
            return reader.readBypassBE<uint32_t>();
        case 3:
            return reader.readBypassBE<uint64_t>();
        default:
            return 0;
    }
}

TileStructure::TileStructure() : variable_size_tiles(false), n_tiles(0), start_index{}, end_index{}, tile_size{} {}

TileStructure::TileStructure(util::BitReader& reader, uint8_t ATCoordSize, bool two_dimensional) {
    read(reader, ATCoordSize, two_dimensional);
}
TileStructure::TileStructure(bool variable_size_tiles, uint64_t n_tiles, std::vector<std::vector<uint64_t>> start_index,
                             std::vector<std::vector<uint64_t>> end_index, std::vector<uint64_t> tile_size)
    : variable_size_tiles(variable_size_tiles),
      n_tiles(n_tiles),
      start_index(start_index),
      end_index(end_index),
      tile_size(tile_size) {}

void TileStructure::read(util::BitReader& reader, uint8_t ATCoordSize, bool two_dimensional) {
    reader.read_b(7);
    variable_size_tiles = static_cast<bool>(reader.read_b(1));
    n_tiles = readVariableSize(reader, ATCoordSize);
    auto dimensions = two_dimensional ? 2 : 1;
    if (variable_size_tiles) {
        start_index.resize(n_tiles, std::vector<uint64_t>(dimensions));
        end_index.resize(n_tiles, std::vector<uint64_t>(dimensions));
        for (auto i = 0; i, n_tiles; ++i) {
            for (auto j = 0; j < dimensions; ++j) {
                start_index[i][j] = readVariableSize(reader, ATCoordSize);
                end_index[i][j] = readVariableSize(reader, ATCoordSize);
            }
        }
    } else {
        tile_size.resize(two_dimensional);
        for (auto& dimension : tile_size) dimension = readVariableSize(reader, ATCoordSize);
    }
}

void TileStructure::write(std::ostream& outputfile, bool two_dimensional) {
    outputfile << std::to_string(variable_size_tiles) << ",";
    outputfile << std::to_string(n_tiles) << ",";
    auto dimensions = two_dimensional ? 2 : 1;
    if (variable_size_tiles) {
        for (auto i = 0; i < n_tiles; ++i) {
            for (auto j = 0; j < dimensions; ++j) {
                outputfile << std::to_string(start_index[i][j]) << ",";
                outputfile << std::to_string(end_index[i][j]) << ",";
            }
        }
    } else
        for (auto j = 0; j < dimensions; ++j) outputfile << std::to_string(tile_size[j]) << ",";
}

Record::Record()
    : AG_class(0),
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

Record::Record(uint8_t AG_class, bool attribute_contiguity, bool two_dimensional, bool column_major_tile_order,
               uint8_t symmetry_mode, bool symmetry_minor_diagonal, bool attribute_dependent_tiles,
               TileStructure default_tile_structure, uint16_t n_add_tile_structures, std::vector<uint16_t> n_attributes,
               std::vector<std::vector<uint16_t>> attribute_ID, std::vector<uint8_t> n_descriptors,
               std::vector<std::vector<uint8_t>> descriptor_ID, std::vector<TileStructure> additional_tile_structure)
    : AG_class(AG_class),
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
      additional_tile_structure(additional_tile_structure) {}

Record::Record(util::BitReader& reader, uint8_t ATCoordSize) { read(reader, ATCoordSize); }

void Record::read(util::BitReader& reader, uint8_t ATCoordSize) {
    AG_class = static_cast<uint8_t>(reader.read_b(3));
    attribute_contiguity = static_cast<uint8_t>(reader.read_b(1));
    two_dimensional = static_cast<uint8_t>(reader.read_b(1));
    if (two_dimensional) {
        static_cast<uint8_t>(reader.read_b(6));
        column_major_tile_order = static_cast<uint8_t>(reader.read_b(1));
        symmetry_mode = static_cast<uint8_t>(reader.read_b(3));
        symmetry_minor_diagonal = static_cast<uint8_t>(reader.read_b(1));
    } else {
        static_cast<uint8_t>(reader.read_b(3));
    }
    attribute_dependent_tiles = static_cast<bool>(reader.read_b(1));
    default_tile_structure.read(reader, ATCoordSize, two_dimensional);
    if (attribute_dependent_tiles) {
        n_add_tile_structures = reader.readBypassBE<uint16_t>();
        n_attributes.resize(n_add_tile_structures);
        attribute_ID.resize(n_add_tile_structures);
        n_descriptors.resize(n_add_tile_structures);
        descriptor_ID.resize(n_add_tile_structures);
        additional_tile_structure.resize(n_add_tile_structures);
        for (auto i = 0; i < n_add_tile_structures; ++i) {
            n_attributes[i] = reader.readBypassBE<uint16_t>();
            for (auto j = 0; j < n_attributes[i]; ++j) {
                attribute_ID[i].push_back(reader.readBypassBE<uint16_t>());
            }
            n_descriptors[i] = static_cast<uint8_t>(reader.read_b(7));
            for (auto j = 0; j < n_descriptors[i]; ++j) {
                descriptor_ID[i].push_back(static_cast<uint8_t>(reader.read_b(7)));
            }
            additional_tile_structure[i].read(reader, ATCoordSize, two_dimensional);
        }
    }
}

void Record::write(std::ostream& outputfile) {
    outputfile << std::to_string(AG_class) << ",";
    outputfile << std::to_string(attribute_contiguity) << ",";
    outputfile << std::to_string(two_dimensional) << ",";
    if (two_dimensional) {
        outputfile << std::to_string(column_major_tile_order) << ",";
        outputfile << std::to_string(symmetry_mode) << ",";
        outputfile << std::to_string(symmetry_minor_diagonal) << ",";
    }
    outputfile << std::to_string(attribute_dependent_tiles) << ",";
    default_tile_structure.write(outputfile, two_dimensional);
    if (attribute_dependent_tiles) {
        outputfile << std::to_string(n_add_tile_structures) << ",";
        for (auto i = 0; i < n_add_tile_structures; ++i) {
            outputfile << std::to_string(n_attributes[i]) << ",";
            for (auto ID : attribute_ID[i]) outputfile << std::to_string(ID) << ",";
            outputfile << std::to_string(n_descriptors[i]) << ",";
            for (auto ID : descriptor_ID[i]) outputfile << std::to_string(ID) << ",";
            additional_tile_structure[i].write(outputfile, two_dimensional);
        }
    }
}

}  // namespace tile_configuration
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
