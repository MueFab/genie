/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_subcm_parameters.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

ContactSubmatParameters::ContactSubmatParameters()
    : chr1_ID(0), chr2_ID(0), tile_parameters(), row_mask_exists_flag(false), col_mask_exists_flag(false) {}

// ---------------------------------------------------------------------------------------------------------------------

ContactSubmatParameters::ContactSubmatParameters(uint8_t _chr1_ID, uint8_t _chr2_ID,
                                                 std::vector<std::vector<TileParameter>>&& _tile_parameters,
                                                 bool _row_mask_exists_flag, bool _col_mask_exists_flag)
    : chr1_ID(_chr1_ID),
      chr2_ID(_chr2_ID),
      tile_parameters(_tile_parameters),
      row_mask_exists_flag(_row_mask_exists_flag),
      col_mask_exists_flag(_col_mask_exists_flag) {
    for (size_t i = 0; i < getNTilesInRow(); i++) {
        UTILS_DIE_IF(getNTilesInCol() != tile_parameters[i].size(), "There are less TileParameters than expected!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ContactSubmatParameters::ContactSubmatParameters(util::BitReader& reader, uint8_t _chr1_ID, uint8_t _chr2_ID,
                                                 uint32_t ntiles_in_row, uint32_t ntiles_in_col)
    : chr1_ID(_chr1_ID), chr2_ID(_chr2_ID) {
    tile_parameters.resize(ntiles_in_row);
    for (uint32_t i = 0; i < ntiles_in_row; i++) {
        tile_parameters[i].resize(ntiles_in_col);

        for (uint32_t j = 0; j < ntiles_in_col; j++) {
            if (!(chr1_ID == chr2_ID && i <= j)) {
                auto diag_transform_flag = reader.read<bool>();
                auto diag_tranform_mode = DiagonalTransformMode::MODE_0;
                if (diag_transform_flag) {
                }
                diag_tranform_mode = reader.read<DiagonalTransformMode>(2);
                auto binarization_flag = reader.read<bool>();

                tile_parameters[i][j] = {diag_transform_flag, diag_tranform_mode, binarization_flag};
            }
        }
    }
    row_mask_exists_flag = reader.read<bool>();
    col_mask_exists_flag = reader.read<bool>();
    reader.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactSubmatParameters::getChr1ID() const { return chr1_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactSubmatParameters::getChr2ID() const { return chr2_ID; }

// ---------------------------------------------------------------------------------------------------------------------

ChrIDPair ContactSubmatParameters::getChrPair() { return {chr1_ID, chr2_ID}; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<TileParameter>>& ContactSubmatParameters::getTileParameters() const {
    return tile_parameters;
}

// ---------------------------------------------------------------------------------------------------------------------

bool ContactSubmatParameters::getRowMaskExistsFlat() const { return row_mask_exists_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool ContactSubmatParameters::getColMaskExistsFlat() const { return col_mask_exists_flag; }

// ---------------------------------------------------------------------------------------------------------------------

size_t ContactSubmatParameters::getNTilesInRow() const { return tile_parameters.size(); }

// ---------------------------------------------------------------------------------------------------------------------

size_t ContactSubmatParameters::getNTilesInCol() const { return tile_parameters.front().size(); }

// ---------------------------------------------------------------------------------------------------------------------

// TODO (Yeremia): Implement this!
// void ContactSubmatParameters::write(core::Writer& writer) const {

// ---------------------------------------------------------------------------------------------------------------------

// TODO (Yeremia): Implement this!
// size_t ContactSubmatParameters::getSize(core::Writer& writesize) const { return 0; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
