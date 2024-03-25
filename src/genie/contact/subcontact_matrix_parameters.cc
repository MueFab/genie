/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "subcontact_matrix_parameters.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixParameters::SubcontactMatrixParameters()
    : parameter_set_ID(0),
      chr1_ID(0),
      chr2_ID(0),
      tile_parameters(),
      row_mask_exists_flag(false),
      col_mask_exists_flag(false) {}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixParameters::SubcontactMatrixParameters(SubcontactMatrixParameters&& other) noexcept
    : parameter_set_ID(other.parameter_set_ID),
      chr1_ID(other.chr1_ID),
      chr2_ID(other.chr2_ID),
      tile_parameters(std::move(other.tile_parameters)),
      row_mask_exists_flag(other.row_mask_exists_flag),
      col_mask_exists_flag(other.col_mask_exists_flag) {}

// ---------------------------------------------------------------------------------------------------------------------

// Constructor by reference
SubcontactMatrixParameters::SubcontactMatrixParameters(const SubcontactMatrixParameters& other)
    : parameter_set_ID(other.parameter_set_ID),
      chr1_ID(other.chr1_ID),
      chr2_ID(other.chr2_ID),
      tile_parameters(other.tile_parameters),
      row_mask_exists_flag(other.row_mask_exists_flag),
      col_mask_exists_flag(other.col_mask_exists_flag) {}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixParameters& SubcontactMatrixParameters::operator=(const SubcontactMatrixParameters& other) {
    if (this != &other) {
        parameter_set_ID = other.parameter_set_ID;
        chr1_ID = other.chr1_ID;
        chr2_ID = other.chr2_ID;
        tile_parameters = other.tile_parameters;
        row_mask_exists_flag = other.row_mask_exists_flag;
        col_mask_exists_flag = other.col_mask_exists_flag;
    }
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixParameters::SubcontactMatrixParameters(
    uint8_t _parameter_set_ID,
    uint8_t _chr1_ID,
    uint8_t _chr2_ID,
    std::vector<std::vector<TileParameter>>&& _tile_parameters,
    bool _row_mask_exists_flag,
    bool _col_mask_exists_flag)
    : parameter_set_ID(_parameter_set_ID),
      chr1_ID(_chr1_ID),
      chr2_ID(_chr2_ID),
      tile_parameters(_tile_parameters),
      row_mask_exists_flag(_row_mask_exists_flag),
      col_mask_exists_flag(_col_mask_exists_flag) {
    for (size_t i = 0; i < getNTilesInRow(); i++) {
        UTILS_DIE_IF(getNTilesInCol() != tile_parameters[i].size(), "There are less TileParameters than expected!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixParameters::getParameterSetID() const { return parameter_set_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::setParameterSetID(uint8_t id) { parameter_set_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixParameters::getChr1ID() const { return chr1_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::setChr1ID(uint8_t id) { chr1_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixParameters::getChr2ID() const { return chr2_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::setChr2ID(uint8_t id) { chr2_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<TileParameter>>& SubcontactMatrixParameters::getTileParameters() const {
    return tile_parameters;
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::setTileParameters(const std::vector<std::vector<TileParameter>>& parameters) {
    tile_parameters = parameters;
}

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixParameters::getRowMaskExistsFlag() const { return row_mask_exists_flag; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::setRowMaskExistsFlag(bool flag) { row_mask_exists_flag = flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixParameters::getColMaskExistsFlag() const { return col_mask_exists_flag; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::setColMaskExistsFlag(bool flag) { col_mask_exists_flag = flag; }

// ---------------------------------------------------------------------------------------------------------------------

ChrIDPair SubcontactMatrixParameters::getChrPair() { return {chr1_ID, chr2_ID}; }

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixParameters::isSymmetrical() const { return chr1_ID == chr2_ID;}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixParameters::getNTilesInRow() const { return tile_parameters.size(); }

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixParameters::getNTilesInCol() const { return tile_parameters.front().size(); }

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixParameters::SubcontactMatrixParameters(util::BitReader& reader)
    : parameter_set_ID(reader.readBypassBE<uint8_t>()),
      chr1_ID(reader.readBypassBE<uint8_t>()),
      chr2_ID(reader.readBypassBE<uint8_t>()) {

    // Read the tile parameters
    size_t nrows = reader.readBypassBE<uint32_t>();
    size_t ncols = reader.readBypassBE<uint32_t>();

    tile_parameters.resize(nrows);
    for (size_t i = 0; i < nrows; ++i) {
        tile_parameters[i].resize(ncols);
        for (size_t j = 0; j < ncols; ++j) {

            if (isSymmetrical() && i > j){
                continue;
            }
            // 3 bits reserved + 3 bits for the mode itself
            tile_parameters[i][j].diag_tranform_mode = reader.read<DiagonalTransformMode>(6);
            tile_parameters[i][j].binarization_mode = reader.read<BinarizationMode>(2);
            reader.flush();
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixParameters::getSize() const {
    size_t size = 0;

    // Add the size of the parameter_set_ID, chr1_ID, chr2_ID, row_mask_exists_flag, and col_mask_exists_flag
    size += sizeof(parameter_set_ID);
    size += sizeof(chr1_ID);
    size += sizeof(chr2_ID);

    for (size_t i = 0; i < getNTilesInRow(); ++i) {
        for (size_t j = 0; j < getNTilesInCol(); ++j) {
            if (isSymmetrical() && i > j){
                continue;
            }

            size += 1; // reserved + DiagonalTransformMode + BinarizationMode
        }
    }

    size += sizeof(row_mask_exists_flag);
    size += sizeof(col_mask_exists_flag);

    return size;
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::write(
    util::BitWriter& writer
) const {
    writer.writeBypassBE(parameter_set_ID);
    writer.writeBypassBE(chr1_ID);
    writer.writeBypassBE(chr2_ID);

    // Write the tile_parameters
    for (size_t i = 0; i < getNTilesInRow(); ++i) {
        auto& row = tile_parameters[i];
        for (size_t j = 0; j < getNTilesInCol(); ++j) {
            if (isSymmetrical() && i > j){
                continue;
            }
            auto &tile = row[j];

            writer.write(static_cast<uint8_t>(tile.diag_tranform_mode), 6);
            writer.write(static_cast<uint8_t>(tile.binarization_mode), 2);
        }
    }
    writer.flush();

    writer.writeBypassBE(row_mask_exists_flag);
    writer.writeBypassBE(col_mask_exists_flag);
}

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixParameters::operator==(const SubcontactMatrixParameters& other) const {
    bool ret = parameter_set_ID == other.parameter_set_ID &&
           chr1_ID == other.chr1_ID &&
           chr2_ID == other.chr2_ID &&
           row_mask_exists_flag == other.row_mask_exists_flag &&
           col_mask_exists_flag == other.col_mask_exists_flag &&
           getNTilesInCol() == other.getNTilesInCol() &&
           getNTilesInRow() == other.getNTilesInRow();

    if (ret){
        auto& other_tile_params = other.getTileParameters();
        for (size_t i = 0; i < getNTilesInRow(); ++i) {
            auto& row1 = tile_parameters[i];
            auto& row2 = other_tile_params[i];
            for (size_t j = 0; j < getNTilesInCol(); ++j) {
                if (isSymmetrical() && i > j){
                    continue;
                }
                auto &tile1 = row1[j];
                auto &tile2 = row2[j];

                ret = ret && tile1.diag_tranform_mode == tile2.diag_tranform_mode;
                ret = ret && tile1.binarization_mode == tile2.binarization_mode;
            }
        }
    }

    return ret;
}


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
