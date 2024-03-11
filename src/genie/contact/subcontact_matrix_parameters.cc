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
    : parameter_set_ID(0), chr1_ID(0), chr2_ID(0), tile_parameters(), row_mask_exists_flag(false), col_mask_exists_flag(false) {}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixParameters::SubcontactMatrixParameters(
    uint8_t _parameter_set_ID, uint8_t _chr1_ID, uint8_t _chr2_ID,
    std::vector<std::vector<TileParameter>>&& _tile_parameters,
    bool _row_mask_exists_flag, bool _col_mask_exists_flag)
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

//SubcontactMatrixParameters::SubcontactMatrixParameters(util::BitReader& reader)
//    : parameter_set_ID(reader.read<uint8_t>()),
//      chr1_ID(reader.read<uint8_t>()),
//      chr2_ID(reader.read<uint8_t>()) {
//    tile_parameters.resize(ntiles_in_row);
//    for (uint32_t i = 0; i < ntiles_in_row; i++) {
//        tile_parameters[i].resize(ntiles_in_col);
//
//        for (uint32_t j = 0; j < ntiles_in_col; j++) {
//            if (!(chr1_ID == chr2_ID && i <= j)) {
//                auto diag_transform_flag = reader.read<bool>();
//                auto diag_tranform_mode = DiagonalTransformMode::MODE_0;
//                if (diag_transform_flag) {
//                }
//                diag_tranform_mode = reader.read<DiagonalTransformMode>(2);
//                auto binarization_flag = reader.read<bool>();
//
//                tile_parameters[i][j] = {diag_transform_flag, diag_tranform_mode, binarization_flag};
//            }
//        }
//    }
//    row_mask_exists_flag = reader.read<bool>();
//    col_mask_exists_flag = reader.read<bool>();
//    reader.flush();
//}

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

ChrIDPair SubcontactMatrixParameters::getChrPair() { return {chr1_ID, chr2_ID}; }

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixParameters::isSymmetrical() const { return chr1_ID == chr2_ID;}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixParameters::getNTilesInRow() const { return tile_parameters.size(); }

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixParameters::getNTilesInCol() const { return tile_parameters.front().size(); }

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

// TODO (Yeremia): Implement this!
void SubcontactMatrixParameters::write(core::Writer& writer) const {}

// ---------------------------------------------------------------------------------------------------------------------

// TODO (Yeremia): Implement this!
size_t SubcontactMatrixParameters::getSize(core::Writer& writesize) const { return 0; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
