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
      codec_ID(),
      tile_parameters(),
      row_mask_exists_flag(false),
      col_mask_exists_flag(false)
{}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixParameters::SubcontactMatrixParameters(
    SubcontactMatrixParameters&& other
) noexcept : parameter_set_ID(other.parameter_set_ID),
      chr1_ID(other.chr1_ID),
      chr2_ID(other.chr2_ID),
      codec_ID(other.codec_ID),
      tile_parameters(std::move(other.tile_parameters)),
      row_mask_exists_flag(other.row_mask_exists_flag),
      col_mask_exists_flag(other.col_mask_exists_flag)
{}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixParameters::SubcontactMatrixParameters(
    const SubcontactMatrixParameters& other
) noexcept
    : parameter_set_ID(other.parameter_set_ID),
      chr1_ID(other.chr1_ID),
      chr2_ID(other.chr2_ID),
      codec_ID(other.codec_ID),
      tile_parameters(other.tile_parameters),
      row_mask_exists_flag(other.row_mask_exists_flag),
      col_mask_exists_flag(other.col_mask_exists_flag)
{}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixParameters& SubcontactMatrixParameters::operator=(
    const SubcontactMatrixParameters& other
) {
    if (this != &other) {
        parameter_set_ID = other.parameter_set_ID;
        chr1_ID = other.chr1_ID;
        chr2_ID = other.chr2_ID;
        codec_ID = other.codec_ID;
        tile_parameters = other.tile_parameters;
        row_mask_exists_flag = other.row_mask_exists_flag;
        col_mask_exists_flag = other.col_mask_exists_flag;
    }
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixParameters::SubcontactMatrixParameters(
    util::BitReader& reader,
    ContactMatrixParameters& params
)
    : parameter_set_ID(reader.readBypassBE<uint8_t>()),
      chr1_ID(reader.readBypassBE<uint8_t>()),
      chr2_ID(reader.readBypassBE<uint8_t>())
{

    auto ntiles_in_row = params.getNumTiles(chr1_ID, 1);
    tile_parameters.resize(ntiles_in_row);

    auto ntiles_in_col = params.getNumTiles(chr2_ID, 1);
    for (auto &v: tile_parameters)
        v.resize(ntiles_in_col);

    auto flags = reader.readBypassBE<uint8_t>();
    codec_ID = static_cast<core::AlgoID>(flags & 0x1F);

    for (size_t i = 0u; i < ntiles_in_row; ++i) {
        for (size_t j = 0u; j < ntiles_in_col; ++j) {
            if (isIntraSCM() && i > j){
                continue;
            }

            auto& tile_parameter = tile_parameters[i][j];

            flags = reader.readBypassBE<uint8_t>();
            tile_parameter.diag_tranform_mode = static_cast<DiagonalTransformMode>((flags >> 2) & 0x07);
            tile_parameter.binarization_mode = static_cast<BinarizationMode>(flags & 0x03);
        }
    }

    flags = reader.readBypassBE<uint8_t>();
    row_mask_exists_flag = flags & 0x02;
    col_mask_exists_flag = flags & 0x01;

    reader.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixParameters::operator==(const SubcontactMatrixParameters& other) {
    bool ret = parameter_set_ID == other.parameter_set_ID &&
               chr1_ID == other.chr1_ID &&
               chr2_ID == other.chr2_ID &&
               row_mask_exists_flag == other.row_mask_exists_flag &&
               col_mask_exists_flag == other.col_mask_exists_flag;

    auto ntiles_in_row = getNTilesInRow();
    auto ntiles_in_col = getNTilesInCol();

    ret &= ntiles_in_row && other.getNTilesInRow();
    ret &= ntiles_in_col && other.getNTilesInCol();

    if (ret){
        for (size_t i = 0; i < ntiles_in_row; ++i) {

            for (size_t j = 0; j < ntiles_in_col; ++j) {
                if (isIntraSCM() && i > j){
                    continue;
                }
                auto &tile1 = tile_parameters[i][j];
                auto &tile2 = tile_parameters[i][j];

                ret = ret && tile1.diag_tranform_mode == tile2.diag_tranform_mode;
                ret = ret && tile1.binarization_mode == tile2.binarization_mode;
            }
        }
    }

    return ret;
}


// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixParameters::getParameterSetID() const { return parameter_set_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixParameters::getChr1ID() const { return chr1_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixParameters::getChr2ID() const { return chr2_ID; }

// ---------------------------------------------------------------------------------------------------------------------

core::AlgoID SubcontactMatrixParameters::getCodecID() const {return codec_ID;}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::setParameterSetID(uint8_t id) { parameter_set_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::setChr1ID(uint8_t id) { chr1_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::setChr2ID(uint8_t id) { chr2_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::setCodecID(core::AlgoID _codec_ID) { codec_ID = _codec_ID;}

// ---------------------------------------------------------------------------------------------------------------------

const TileParameters& SubcontactMatrixParameters::getTileParameters() const {
    return tile_parameters;
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::setTileParameters(
    TileParameters&& parameters
) {
    tile_parameters = std::move(parameters);
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

void SubcontactMatrixParameters::setNumTiles(
    size_t ntiles_in_row,
    size_t ntiles_in_col,
    bool free_mem
) {
    if (free_mem){
        tile_parameters.clear();
    }

    tile_parameters.resize(ntiles_in_row);
    for (auto& v: tile_parameters){
        v.resize(ntiles_in_col);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TileParameter& SubcontactMatrixParameters::getTileParameter(
    size_t i_tile,
    size_t j_tile
) {
    UTILS_DIE_IF(i_tile >= getNTilesInRow(), "Invalid i_tile!");
    UTILS_DIE_IF(j_tile >= getNTilesInCol(), "Invalid j_tile!");

    UTILS_DIE_IF(i_tile > j_tile && isIntraSCM(), "Accessing lower triangle of intra SCM is not allowed!");

    return tile_parameters[i_tile][j_tile];
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::setTileParameter(
    size_t i_tile,
    size_t j_tile,
    TileParameter tile_parameter
){
    getTileParameter(i_tile, j_tile) = tile_parameter;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixParameters::getNTilesInRow() const {
    return tile_parameters.size();
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixParameters::getNTilesInCol() const {
    if (getNTilesInRow() > 0)
        return tile_parameters[0].size();
    else
        return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixParameters::isIntraSCM() const { return chr1_ID == chr2_ID;}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixParameters::getSize() const {
    size_t size = 0;

    // Add the size of the parameter_set_ID, chr1_ID, chr2_ID, row_mask_exists_flag, and col_mask_exists_flag
    size += sizeof(parameter_set_ID);
    size += sizeof(chr1_ID);
    size += sizeof(chr2_ID);
    size += 1u; // reserved + codec_ID

    for (size_t i = 0u; i < getNTilesInRow(); ++i) {
        for (size_t j = 0u; j < getNTilesInCol(); ++j) {
            if (isIntraSCM() && i > j){
                continue;
            }

            size += 1u; // reserved + DiagonalTransformMode + BinarizationMode
        }
    }

    size += 1u; // reserved + row_mask_exists_flag + col_mask_exists_flag
//    size += sizeof(row_mask_exists_flag);
//    size += sizeof(col_mask_exists_flag);

    return size;
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixParameters::write(
    util::BitWriter& writer
) const {
    writer.writeBypassBE(parameter_set_ID);
    writer.writeBypassBE(chr1_ID);
    writer.writeBypassBE(chr2_ID);

    uint8_t flags = 0u;
    flags |= (static_cast<uint8_t>(codec_ID) & 0x1F);
    writer.writeBypassBE(flags);

    // Write the tile_parameters
    for (size_t i = 0; i < getNTilesInRow(); ++i) {
        for (size_t j = 0; j < getNTilesInCol(); ++j) {
            if (isIntraSCM() && i > j){
                continue;
            }
            auto & tile_param = tile_parameters[i][j];

            flags = 0u;
            flags |= static_cast<uint8_t>((static_cast<uint8_t>(tile_param.diag_tranform_mode) << 2));
            flags |= static_cast<uint8_t>(tile_param.binarization_mode);
            writer.writeBypassBE(flags);
        }
    }

    flags = 0u;
    flags |= static_cast<uint8_t>((static_cast<uint8_t>(row_mask_exists_flag) << 1));
    flags |= static_cast<uint8_t>(row_mask_exists_flag);
    writer.writeBypassBE(flags);

    writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
