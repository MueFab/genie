/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "subcontact_matrix_parameters.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::contact {

// -----------------------------------------------------------------------------

SubcontactMatrixParameters::SubcontactMatrixParameters()
    : parameter_set_ID_(0),
      chr1_ID_(0),
      chr2_ID_(0),
      codec_ID_(),
      tile_parameters_(),
      row_mask_exists_flag_(false),
      col_mask_exists_flag_(false) {}

// -----------------------------------------------------------------------------

// Parameterized constructor
SubcontactMatrixParameters::SubcontactMatrixParameters(
    uint8_t parameter_set_ID,
    uint8_t chr1_ID,
    uint8_t chr2_ID,
    core::AlgoID codec_ID,
    TileParameters&& tile_parameters,
    bool row_mask_exists_flag,
    bool col_mask_exists_flag)
    : parameter_set_ID_(parameter_set_ID),
      chr1_ID_(chr1_ID),
      chr2_ID_(chr2_ID),
      codec_ID_(codec_ID),
      tile_parameters_(std::move(tile_parameters)),
      row_mask_exists_flag_(row_mask_exists_flag),
      col_mask_exists_flag_(col_mask_exists_flag) {
}

// -----------------------------------------------------------------------------

//SubcontactMatrixParameters::SubcontactMatrixParameters(const SubcontactMatrixParameters& other) noexcept = default;
SubcontactMatrixParameters& SubcontactMatrixParameters::operator=(
    const SubcontactMatrixParameters& other) {
  if (this == &other) return *this;
  parameter_set_ID_ = other.parameter_set_ID_;
  chr1_ID_ = other.chr1_ID_;
  chr2_ID_ = other.chr2_ID_;
  codec_ID_ = other.codec_ID_;
  tile_parameters_ = other.tile_parameters_;
  row_mask_exists_flag_ = other.row_mask_exists_flag_;
  col_mask_exists_flag_ = other.col_mask_exists_flag_;
  return *this;
}

// -----------------------------------------------------------------------------

SubcontactMatrixParameters::SubcontactMatrixParameters(SubcontactMatrixParameters&& other) noexcept
    : parameter_set_ID_(other.parameter_set_ID_),
      chr1_ID_(other.chr1_ID_),
      chr2_ID_(other.chr2_ID_),
      codec_ID_(other.codec_ID_),
      tile_parameters_(std::move(other.tile_parameters_)),
      row_mask_exists_flag_(other.row_mask_exists_flag_),
      col_mask_exists_flag_(other.col_mask_exists_flag_) {}

// -----------------------------------------------------------------------------

// Move assignment operator
SubcontactMatrixParameters& SubcontactMatrixParameters::operator=(
    SubcontactMatrixParameters&& other) noexcept {
    if (this == &other) return *this;
    parameter_set_ID_ = std::move(other.parameter_set_ID_);
    chr1_ID_ = std::move(other.chr1_ID_);
    chr2_ID_ = std::move(other.chr2_ID_);
    codec_ID_ = std::move(other.codec_ID_);
    tile_parameters_ = std::move(other.tile_parameters_);
    row_mask_exists_flag_ = std::move(other.row_mask_exists_flag_);
    col_mask_exists_flag_ = std::move(other.col_mask_exists_flag_);
    return *this;
}

// -----------------------------------------------------------------------------

SubcontactMatrixParameters::SubcontactMatrixParameters(util::BitReader& reader, ContactMatrixParameters& cm_params)
    : parameter_set_ID_(reader.ReadAlignedInt<uint8_t>()),
      chr1_ID_(reader.ReadAlignedInt<uint8_t>()),
      chr2_ID_(reader.ReadAlignedInt<uint8_t>()) {
    auto ntiles_in_row = cm_params.GetNumTiles(chr1_ID_, 1);
    tile_parameters_.resize(ntiles_in_row);

    auto ntiles_in_col = cm_params.GetNumTiles(chr2_ID_, 1);
    for (auto& v : tile_parameters_) v.resize(ntiles_in_col);

    auto flags = reader.ReadAlignedInt<uint8_t>();
    codec_ID_ = static_cast<core::AlgoID>(flags & 0x1F);

    for (size_t i = 0u; i < ntiles_in_row; ++i) {
        for (size_t j = 0u; j < ntiles_in_col; ++j) {
            if (IsIntraSCM() && i > j) {
                continue;
            }

            auto& tile_parameter = tile_parameters_[i][j];

            flags = reader.ReadAlignedInt<uint8_t>();
            tile_parameter.diag_tranform_mode = static_cast<DiagonalTransformMode>((flags >> 2) & 0x07);
            tile_parameter.binarization_mode = static_cast<BinarizationMode>(flags & 0x03);
        }
    }

    flags = reader.ReadAlignedInt<uint8_t>();
    row_mask_exists_flag_ = flags & 0x02;
    col_mask_exists_flag_ = flags & 0x01;

    reader.FlushHeldBits();
}

// -----------------------------------------------------------------------------

bool SubcontactMatrixParameters::operator==(const SubcontactMatrixParameters& other) {
    bool ret = parameter_set_ID_ == other.parameter_set_ID_ &&
             chr1_ID_ == other.chr1_ID_ && chr2_ID_ == other.chr2_ID_ &&
             row_mask_exists_flag_ == other.row_mask_exists_flag_ &&
             col_mask_exists_flag_ == other.col_mask_exists_flag_;

    auto ntiles_in_row = GetNTilesInRow();
    auto ntiles_in_col = GetNTilesInCol();

    ret &= ntiles_in_row && other.GetNTilesInRow();
    ret &= ntiles_in_col && other.GetNTilesInCol();

    if (ret) {
        for (size_t i = 0; i < ntiles_in_row; ++i) {
            for (size_t j = 0; j < ntiles_in_col; ++j) {
                if (IsIntraSCM() && i > j) {
                    continue;
                }
                auto& tile1 = tile_parameters_[i][j];
                auto& tile2 = tile_parameters_[i][j];

                ret = ret && tile1.diag_tranform_mode == tile2.diag_tranform_mode;
                ret = ret && tile1.binarization_mode == tile2.binarization_mode;
            }
        }
    }

    return ret;
}

// -----------------------------------------------------------------------------

uint8_t SubcontactMatrixParameters::GetParameterSetID() const { return parameter_set_ID_; }

// -----------------------------------------------------------------------------

uint8_t SubcontactMatrixParameters::GetChr1ID() const { return chr1_ID_; }

// -----------------------------------------------------------------------------

uint8_t SubcontactMatrixParameters::GetChr2ID() const { return chr2_ID_; }

// -----------------------------------------------------------------------------

core::AlgoID SubcontactMatrixParameters::GetCodecID() const { return codec_ID_; }

// -----------------------------------------------------------------------------

[[maybe_unused]] void SubcontactMatrixParameters::SetParameterSetID(uint8_t ID) {
  parameter_set_ID_ = ID; }

// -----------------------------------------------------------------------------

void SubcontactMatrixParameters::SetChr1ID(uint8_t ID) { chr1_ID_ = ID; }

// -----------------------------------------------------------------------------

void SubcontactMatrixParameters::SetChr2ID(uint8_t ID) { chr2_ID_ = ID; }

// -----------------------------------------------------------------------------

void SubcontactMatrixParameters::SetCodecID(core::AlgoID codec_ID) { codec_ID_ = codec_ID; }

// -----------------------------------------------------------------------------

[[maybe_unused]] const TileParameters& SubcontactMatrixParameters::GetTileParameters() const { return tile_parameters_; }

// -----------------------------------------------------------------------------

[[maybe_unused]] void SubcontactMatrixParameters::SetTileParameters(TileParameters&& parameters) {
  tile_parameters_ = std::move(parameters);
}

// -----------------------------------------------------------------------------

bool SubcontactMatrixParameters::GetRowMaskExistsFlag() const { return row_mask_exists_flag_; }

// -----------------------------------------------------------------------------

void SubcontactMatrixParameters::SetRowMaskESetRowMaskExistsFlag(bool flag) {
  row_mask_exists_flag_ = flag; }

// -----------------------------------------------------------------------------

bool SubcontactMatrixParameters::GetColMaskExistsFlag() const { return col_mask_exists_flag_; }

// -----------------------------------------------------------------------------

void SubcontactMatrixParameters::SetColMaskExistsFlag(bool flag) {
  col_mask_exists_flag_ = flag; }

// -----------------------------------------------------------------------------

void SubcontactMatrixParameters::SetNumTiles(size_t ntiles_in_row, size_t ntiles_in_col, bool free_mem) {
    if (free_mem) {
      tile_parameters_.clear();
    }

    tile_parameters_.resize(ntiles_in_row);
    for (auto& v : tile_parameters_) {
        v.resize(ntiles_in_col);
    }
}

// -----------------------------------------------------------------------------

TileParameter& SubcontactMatrixParameters::GetTileParameter(size_t i_tile, size_t j_tile) {
    UTILS_DIE_IF(i_tile >= GetNTilesInRow(), "Invalid i_tile!");
    UTILS_DIE_IF(j_tile >= GetNTilesInCol(), "Invalid j_tile!");
    UTILS_DIE_IF(i_tile > j_tile && IsIntraSCM(), "Accessing lower triangle of intra SCM is not allowed!");

    return tile_parameters_[i_tile][j_tile];
}

// -----------------------------------------------------------------------------

void SubcontactMatrixParameters::SetTileParameter(size_t i_tile, size_t j_tile, TileParameter tile_parameter) {
    //    UTILS_DIE_IF(i_tile >= GetNTilesInRow(), "Invalid i_tile!");
    //    UTILS_DIE_IF(j_tile >= GetNTilesInCol(), "Invalid j_tile!");
    //    UTILS_DIE_IF(i_tile > j_tile && IsIntraSCM(), "Accessing lower triangle of intra SCM is not allowed!");

    GetTileParameter(i_tile, j_tile) = tile_parameter;
}

// -----------------------------------------------------------------------------

size_t SubcontactMatrixParameters::GetNTilesInRow() const { return tile_parameters_.size(); }

// -----------------------------------------------------------------------------

size_t SubcontactMatrixParameters::GetNTilesInCol() const {
    if (GetNTilesInRow() > 0)
        return tile_parameters_[0].size();
    else
        return 0;
}

// -----------------------------------------------------------------------------

bool SubcontactMatrixParameters::IsIntraSCM() const { return chr1_ID_ == chr2_ID_; }

// -----------------------------------------------------------------------------

size_t SubcontactMatrixParameters::GetSize() const {
    size_t size = 0;

    // Add the size of the parameter_set_ID_, chr1_ID_, chr2_ID_, row_mask_exists_flag_, and col_mask_exists_flag_
    size += sizeof(parameter_set_ID_);
    size += sizeof(chr1_ID_);
    size += sizeof(chr2_ID_);
    size += sizeof(codec_ID_);  // reserved + codec_ID_

    for (size_t i = 0u; i < GetNTilesInRow(); ++i) {
        for (size_t j = 0u; j < GetNTilesInCol(); ++j) {
            if (IsIntraSCM() && i > j) {
                continue;
            }

            size += 1u;  // reserved + DiagonalTransformMode + BinarizationMode
        }
    }

    size += 1u;  // reserved + row_mask_exists_flag_ + col_mask_exists_flag_
                 //    size += sizeof(row_mask_exists_flag_);
                 //    size += sizeof(col_mask_exists_flag_);

    return size;
}

// -----------------------------------------------------------------------------

void SubcontactMatrixParameters::Write(util::BitWriter& writer) const {
    writer.WriteBypassBE(parameter_set_ID_);
    writer.WriteBypassBE(chr1_ID_);
    writer.WriteBypassBE(chr2_ID_);

    uint8_t flags = 0u;
    flags |= (static_cast<uint8_t>(codec_ID_) & 0x1F);
    writer.WriteBypassBE(flags);

    // Write the tile_parameters_
    for (size_t i = 0; i < GetNTilesInRow(); ++i) {
        for (size_t j = 0; j < GetNTilesInCol(); ++j) {
            if (IsIntraSCM() && i > j) {
                continue;
            }
            auto& tile_param = tile_parameters_[i][j];

            flags = 0u;
            flags |= static_cast<uint8_t>((static_cast<uint8_t>(tile_param.diag_tranform_mode) << 2));
            flags |= static_cast<uint8_t>(tile_param.binarization_mode);
            writer.WriteBypassBE(flags);
        }
    }

    flags = 0u;
    flags |= static_cast<uint8_t>((static_cast<uint8_t>(row_mask_exists_flag_) << 1));
    flags |= static_cast<uint8_t>(row_mask_exists_flag_);
    writer.WriteBypassBE(flags);

    writer.FlushBits();
}

// -----------------------------------------------------------------------------

void SubcontactMatrixParameters::Write(core::Writer& writer) const {
  writer.Write(parameter_set_ID_, 8);
    writer.Write(chr1_ID_, 8);
    writer.Write(chr2_ID_, 8);
    writer.WriteReserved(3);
    uint8_t flags = 0u;
    flags |= (static_cast<uint8_t>(codec_ID_) & 0x1F);
    writer.Write(flags, 5);

    // Write the tile_parameters_
    for (size_t i = 0; i < GetNTilesInRow(); ++i) {
        for (size_t j = 0; j < GetNTilesInCol(); ++j) {
            if (IsIntraSCM() && i > j) {
                continue;
            }
            auto& tile_param = tile_parameters_[i][j];
            writer.WriteReserved(3);
            writer.Write(static_cast<uint8_t>(tile_param.diag_tranform_mode),
                         3);
            writer.Write(static_cast<uint8_t>(tile_param.binarization_mode), 2);
        }
    }
    writer.WriteReserved(6);
    writer.Write(row_mask_exists_flag_, 1);
    writer.Write(col_mask_exists_flag_, 1);
}

// -----------------------------------------------------------------------------

}  // namespace genie::contact

// -----------------------------------------------------------------------------
