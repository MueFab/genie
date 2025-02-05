/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include "subcontact_matrix_payload.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixPayload::SubcontactMatrixPayload()
    : parameter_set_ID_(0),
      sample_ID_(0),
      chr1_ID_(0),
      chr2_ID_(0),
      tile_payloads_(),
      row_mask_payload_(),
      col_mask_payload_() {}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] SubcontactMatrixPayload::SubcontactMatrixPayload(
    uint8_t _parameter_set_ID,
    uint16_t _sample_ID,
    uint8_t _chr1_ID,
    uint8_t _chr2_ID
    ) : parameter_set_ID_(_parameter_set_ID),
      sample_ID_(_sample_ID),
      chr1_ID_(_chr1_ID),
      chr2_ID_(_chr2_ID){}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] SubcontactMatrixPayload::SubcontactMatrixPayload(
    uint8_t _parameter_set_ID,
    uint16_t _sample_ID,
    uint8_t _chr1_ID,
    uint8_t _chr2_ID,
    TilePayloads&& _tile_payloads,
    std::optional<SubcontactMatrixMaskPayload>&& _row_mask_payload,
    std::optional<SubcontactMatrixMaskPayload>&& _col_mask_payload
    ) : parameter_set_ID_(_parameter_set_ID),
      sample_ID_(_sample_ID),
      chr1_ID_(_chr1_ID),
      chr2_ID_(_chr2_ID),
      tile_payloads_(std::move(_tile_payloads)),
      row_mask_payload_(_row_mask_payload),
      col_mask_payload_(_col_mask_payload) {}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixPayload::SubcontactMatrixPayload(
    util::BitReader &reader,
    ContactMatrixParameters& cm_param,
    const SubcontactMatrixParameters& scm_param
){

    auto MULT = 1u;

    UTILS_DIE_IF(!reader.IsByteAligned(),  "Not byte aligned!");

    parameter_set_ID_ = reader.ReadAlignedInt<uint8_t>();
    sample_ID_ = reader.ReadAlignedInt<uint16_t>();
    chr1_ID_ = reader.ReadAlignedInt<uint8_t>();
    chr2_ID_ = reader.ReadAlignedInt<uint8_t>();

    UTILS_DIE_IF(parameter_set_ID_ != scm_param.GetParameterSetID(),
        "parameter_set_ID_ differs"
    );
    UTILS_DIE_IF(chr1_ID_ != scm_param.GetChr1ID(),
        "chr1_ID_ differs"
    );
    UTILS_DIE_IF(chr2_ID_ != scm_param.GetChr2ID(),
        "chr2_ID_ differs"
    );

    auto ntiles_in_row = cm_param.getNumTiles(chr1_ID_, MULT);
    auto ntiles_in_col = cm_param.getNumTiles(chr2_ID_, MULT);

    UTILS_DIE_IF(ntiles_in_row != scm_param.GetNTilesInRow(), "chr1_ID_ differs");
    UTILS_DIE_IF(ntiles_in_col != scm_param.GetNTilesInCol(), "chr2_ID_ differs");

    SetNumTiles(ntiles_in_row, ntiles_in_col);

    for (auto i = 0u; i< GetNTilesInRow(); i++){
        for (auto j = 0u; j< GetNTilesInCol(); j++){
            if (!IsIntraScm() || i<=j){

                auto tile_payload_size = reader.ReadAlignedInt<uint32_t>();
                auto tile_payload = ContactMatrixTilePayload(reader, tile_payload_size);
                UTILS_DIE_IF(
                    tile_payload_size != tile_payload.getSize(),
                    "Invalid tile_payload size!"
                );
                SetTilePayload(i, j, std::move(tile_payload));
            }

        }
    }

    //TODO(yeremia): Missing norm_matrices

    if (scm_param.GetRowMaskExistsFlag()){
        auto num_entries = static_cast<uint32_t>(cm_param.getNumBinEntries(chr1_ID_, MULT));
        auto mask_payload_size = reader.ReadAlignedInt<uint32_t>();
        auto mask_payload = SubcontactMatrixMaskPayload(
            reader,
            num_entries
        );
        UTILS_DIE_IF(
            mask_payload_size != mask_payload.getSize(),
            "Invalid mask_payload_size"
        );
        SetRowMaskPayload(std::move(mask_payload));
    }
    if (!IsIntraScm() && scm_param.GetColMaskExistsFlag()){
        auto num_entries = cm_param.getNumBinEntries(chr2_ID_, MULT);
        auto mask_payload_size = reader.ReadAlignedInt<uint32_t>();
        auto mask_payload = SubcontactMatrixMaskPayload(
            reader,
            static_cast<uint32_t>(num_entries)
        );
        UTILS_DIE_IF(
            mask_payload_size != mask_payload.getSize(),
            "Invalid mask_payload_size"
        );
        SetColMaskPayload(std::move(mask_payload));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

//TOOD(yeremia): rewrite is as friend operator
bool SubcontactMatrixPayload::operator==(
    SubcontactMatrixPayload& other
) {
    auto ret =
      parameter_set_ID_ == other.parameter_set_ID_ &&
      sample_ID_ == other.sample_ID_ && chr1_ID_ == other.chr1_ID_ &&
      chr2_ID_ == other.chr2_ID_ && tile_payloads_ == other.tile_payloads_ &&
      row_mask_payload_.has_value() == other.row_mask_payload_.has_value() &&
      col_mask_payload_.has_value() == other.col_mask_payload_.has_value();

    if (ret && row_mask_payload_.has_value()){
        ret = row_mask_payload_.value() == other.row_mask_payload_.value();
    }

    if (ret && col_mask_payload_.has_value()){
        ret = col_mask_payload_.value() == other.col_mask_payload_.value();
    }

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] uint8_t SubcontactMatrixPayload::GetParameterSetId() const { return parameter_set_ID_; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t SubcontactMatrixPayload::GetSampleID() const { return sample_ID_; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixPayload::GetChr1ID() const { return chr1_ID_; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixPayload::GetChr2ID() const { return chr2_ID_; }

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] const TilePayloads& SubcontactMatrixPayload::GetTilePayloads() const { return tile_payloads_; }

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] size_t SubcontactMatrixPayload::GetNumNormMatrices() const { return norm_tile_payloads_.size(); }

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixPayload::AnyRowMaskPayload() const { return row_mask_payload_.has_value();}

// ---------------------------------------------------------------------------------------------------------------------

const SubcontactMatrixMaskPayload& SubcontactMatrixPayload::GetRowMaskPayload() const {
    UTILS_DIE_IF(
        !AnyRowMaskPayload(),
        "Row mask does not exists!"
    );

    return *row_mask_payload_;
}

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixPayload::AnyColMaskPayload() const { return col_mask_payload_.has_value();}

// ---------------------------------------------------------------------------------------------------------------------

const SubcontactMatrixMaskPayload& SubcontactMatrixPayload::GetColMaskPayload() const {
    UTILS_DIE_IF(
        !AnyColMaskPayload(),
        "Column mask does not exists!"
    );

    return *col_mask_payload_;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void SubcontactMatrixPayload::SetParameterSetId(uint8_t id) {
  parameter_set_ID_ = id; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::SetSampleId(uint16_t id) { sample_ID_ = id; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::SetChr1Id(uint8_t id) { chr1_ID_ = id; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::SetChr2Id(uint8_t id) { chr2_ID_ = id; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::SetTilePayloads(const TilePayloads& payloads) {}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::SetRowMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload) {
  row_mask_payload_ = payload;
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::SetColMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload) {
    // Set the column mask is only allowed for inter SCM
    if (!IsIntraScm()){
      col_mask_payload_ = payload;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::SetNumTiles(
    size_t ntiles_in_row,
    size_t ntiles_in_col,
    bool free_mem
) {
    if (free_mem){
      tile_payloads_.clear();
    }

    tile_payloads_.resize(ntiles_in_row);
    for (auto& v: tile_payloads_){
        v.resize(ntiles_in_col);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixTilePayload& SubcontactMatrixPayload::GetTilePayload(size_t i_tile, size_t j_tile){
    UTILS_DIE_IF(i_tile >= GetNTilesInRow(), "i_tile is greater than ntiles_in_row");
    UTILS_DIE_IF(j_tile >= GetNTilesInCol(), "j_tile is greater than ntiles_in_col");
    UTILS_DIE_IF(i_tile > j_tile && IsIntraScm(), "Accessing lower triangle of intra SCM is not allowed!");

    return tile_payloads_[i_tile][j_tile];
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::SetTilePayload(
    size_t i_tile,
    size_t j_tile,
    ContactMatrixTilePayload&& tile_payload
) {
  GetTilePayload(i_tile, j_tile) = std::move(tile_payload);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixPayload::GetNTilesInRow() const {
    return tile_payloads_.size();
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixPayload::GetNTilesInCol() const {
    if (GetNTilesInRow() > 0)
        return tile_payloads_[0].size();
    else
        return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixPayload::IsIntraScm() const{
    return chr1_ID_ == chr2_ID_;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixPayload::GetSize() const{
    size_t size = sizeof(uint8_t); // parameter_set_ID_
    size += sizeof(uint16_t ); // sample_ID_
    size += sizeof(uint8_t ); // chr1_ID_
    size += sizeof(uint8_t ); // chr2_ID_

    auto ntiles_in_row = GetNTilesInRow();
    auto ntiles_in_col = GetNTilesInCol();
    for (auto i = 0u; i< ntiles_in_row; i++){
        for (auto j = 0u; j< ntiles_in_col; j++){
            if (!IsIntraScm() || i<=j){
                size += TILE_PAYLOAD_SIZE_LEN;
                size += tile_payloads_[i][j].getSize();
            }
        }
    }

    // TODO (Yeremia): Missing norm_matrices

    if (row_mask_payload_.has_value()) {
        size += MASK_PAYLOAD_SIZE_LEN;
        size += row_mask_payload_->getSize();
    }

    if (!IsIntraScm() && col_mask_payload_.has_value()) {
        size += MASK_PAYLOAD_SIZE_LEN;
        size += col_mask_payload_->getSize();
    }

    return size;
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::Write(util::BitWriter &writer) const{
    writer.WriteBypassBE(parameter_set_ID_);
    writer.WriteBypassBE(sample_ID_);
    writer.WriteBypassBE(chr1_ID_);
    writer.WriteBypassBE(chr2_ID_);

    for (auto i = 0u; i< GetNTilesInRow(); i++){
        for (auto j = 0u; j< GetNTilesInCol(); j++){
            if (!IsIntraScm() || i<=j){
                auto& tile_payload = tile_payloads_[i][j];
                auto tile_payload_size = tile_payload.getSize();

                writer.WriteBypassBE(static_cast<uint32_t>(tile_payload_size));

                tile_payload.write(writer);
            }
        }
    }

    // TODO (Yeremia): Missing norm_matrices
//    for (auto& v: norm_tile_payloads_){
//
//    }

    if (row_mask_payload_.has_value()){
        auto row_mask_payload_size = row_mask_payload_->getSize();
        writer.WriteBypassBE(static_cast<uint32_t>(row_mask_payload_size));
        row_mask_payload_->write(writer);
    }

    if (!IsIntraScm() && col_mask_payload_.has_value()){
        auto col_mask_payload_size = col_mask_payload_->getSize();
        writer.WriteBypassBE(static_cast<uint32_t>(col_mask_payload_size));
        col_mask_payload_->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}
}

// ---------------------------------------------------------------------------------------------------------------------
