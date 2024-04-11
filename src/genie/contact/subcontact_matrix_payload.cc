/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include "subcontact_matrix_payload.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixPayload::SubcontactMatrixPayload()
    : parameter_set_ID(0),
      sample_ID(0),
      chr1_ID(0),
      chr2_ID(0),
      tile_payloads(),
      row_mask_payload(),
      col_mask_payload() {}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixPayload::SubcontactMatrixPayload(
    uint8_t _parameter_set_ID,
    uint8_t _sample_ID,
    uint8_t _chr1_ID,
    uint8_t _chr2_ID
    ) : parameter_set_ID(_parameter_set_ID),
        sample_ID(_sample_ID),
        chr1_ID(_chr1_ID),
        chr2_ID(_chr2_ID){}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixPayload::SubcontactMatrixPayload(
    uint8_t _parameter_set_ID,
    uint8_t _sample_ID,
    uint8_t _chr1_ID,
    uint8_t _chr2_ID,
    TilePayloads&& _tile_payloads,
    std::optional<SubcontactMatrixMaskPayload>&& _row_mask_payload,
    std::optional<SubcontactMatrixMaskPayload>&& _col_mask_payload
    ) : parameter_set_ID(_parameter_set_ID),
        sample_ID(_sample_ID),
        chr1_ID(_chr1_ID),
        chr2_ID(_chr2_ID),
        tile_payloads(std::move(_tile_payloads)),
        row_mask_payload(_row_mask_payload),
        col_mask_payload(_col_mask_payload) {}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixPayload::SubcontactMatrixPayload(
    util::BitReader &reader,
    ContactMatrixParameters& cm_param,
    const SubcontactMatrixParameters& scm_param
) noexcept{

    auto MULT = 1u;

    UTILS_DIE_IF(!reader.isAligned(),  "Not byte aligned!");

    parameter_set_ID = reader.readBypassBE<uint8_t>();
    sample_ID = reader.readBypassBE<uint8_t>();
    chr1_ID = reader.readBypassBE<uint8_t>();
    chr2_ID = reader.readBypassBE<uint8_t>();

    UTILS_DIE_IF(
        parameter_set_ID != scm_param.getParameterSetID(),
        "parameter_set_ID differs"
    );
    UTILS_DIE_IF(
        chr1_ID != scm_param.getChr1ID(),
        "chr1_ID differs"
    );
    UTILS_DIE_IF(
        chr2_ID != scm_param.getChr2ID(),
        "chr2_ID differs"
    );

    auto ntiles_in_row = cm_param.getNumTiles(chr1_ID, MULT);
    auto ntiles_in_col = cm_param.getNumTiles(chr2_ID, MULT);

    UTILS_DIE_IF(ntiles_in_row != scm_param.getNTilesInRow(), "chr1_ID differs");
    UTILS_DIE_IF(ntiles_in_col != scm_param.getNTilesInCol(), "chr2_ID differs");

    setNumTiles(ntiles_in_row, ntiles_in_col);

    for (auto i = 0u; i<getNTilesInRow(); i++){
        for (auto j = 0u; j<getNTilesInCol(); j++){
            if (!(isIntraSCM() && i>j)){

                auto tile_payload_size = reader.readBypassBE<uint32_t>();
                auto tile_payload = ContactMatrixTilePayload(reader);
                UTILS_DIE_IF(
                    tile_payload_size != tile_payload.getSize(),
                    "Invalid tile_payload size!"
                );
                setTilePayload(i, j, std::move(tile_payload));
            }

        }
    }

    //TODO(yeremia): Missing norm_matrices

    if (scm_param.getRowMaskExistsFlag()){
        auto row_mask_payload_len = reader.read<uint32_t>();

    }

    if (scm_param.getRowMaskExistsFlag()){
        auto num_entries = cm_param.getNumBinEntries(chr1_ID, MULT);
        auto mask_payload_size = reader.readBypassBE<uint32_t>();
        auto mask_payload = SubcontactMatrixMaskPayload(
            reader,
            num_entries
        );
        UTILS_DIE_IF(mask_payload_size != mask_payload.getSize(), "Invalid mask_payload_size");
        setRowMaskPayload(std::move(mask_payload));
    }
    if (!isIntraSCM() && scm_param.getColMaskExistsFlag()){
        auto num_entries = cm_param.getNumBinEntries(chr2_ID, MULT);
        auto mask_payload_size = reader.readBypassBE<uint32_t>();
        auto mask_payload = SubcontactMatrixMaskPayload(
            reader,
            num_entries
        );
        UTILS_DIE_IF(mask_payload_size != mask_payload.getSize(), "Invalid mask_payload_size");
        setRowMaskPayload(std::move(mask_payload));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixPayload::operator==(
    const SubcontactMatrixPayload& other
) {
    return parameter_set_ID == other.parameter_set_ID &&
           sample_ID == other.sample_ID &&
           chr1_ID == other.chr1_ID &&
           chr2_ID == other.chr2_ID &&
           tile_payloads == other.tile_payloads &&
           row_mask_payload.has_value() == other.row_mask_payload.has_value() &&
           col_mask_payload.has_value() == other.col_mask_payload.has_value() &&
           row_mask_payload.value() == other.row_mask_payload.value() &&
           col_mask_payload.value() == other.col_mask_payload.value();
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixPayload::getParameterSetID() const { return parameter_set_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixPayload::getSampleID() const { return sample_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixPayload::getChr1ID() const { return chr1_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixPayload::getChr2ID() const { return chr2_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const TilePayloads& SubcontactMatrixPayload::getTilePayloads() const { return tile_payloads; }

// ---------------------------------------------------------------------------------------------------------------------

const size_t SubcontactMatrixPayload::getNumNormMatrices() const { return norm_tile_payloads.size(); };

// ---------------------------------------------------------------------------------------------------------------------

const std::optional<SubcontactMatrixMaskPayload>& SubcontactMatrixPayload::getRowMaskPayload() const { return row_mask_payload; }

// ---------------------------------------------------------------------------------------------------------------------

const std::optional<SubcontactMatrixMaskPayload>& SubcontactMatrixPayload::getColMaskPayload() const { return col_mask_payload; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setParameterSetID(uint8_t id) { parameter_set_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setSampleID(uint8_t id) { sample_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setChr1ID(uint8_t id) { chr1_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setChr2ID(uint8_t id) { chr2_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

//void SubcontactMatrixPayload::setTilePayloads(TilePayloads&& payloads) { tile_payloads = payloads; }

// ---------------------------------------------------------------------------------------------------------------------

//void SubcontactMatrixPayload::setRowMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload) { row_mask_payload = payload; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setRowMaskPayload(SubcontactMatrixMaskPayload&& payload){ row_mask_payload = payload;}

// ---------------------------------------------------------------------------------------------------------------------

//void SubcontactMatrixPayload::setColMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload) { col_mask_payload = payload; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setColMaskPayload(SubcontactMatrixMaskPayload&& payload){ col_mask_payload = payload;}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setNumTiles(
    size_t ntiles_in_row,
    size_t ntiles_in_col,
    bool free_mem
) {
    if (free_mem){
        tile_payloads.clear();
    }

    tile_payloads.resize(ntiles_in_row);
    for (auto& v:tile_payloads){
        v.resize(ntiles_in_col);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setTilePayload(
    size_t i_tile,
    size_t j_tile,
    ContactMatrixTilePayload&& tile_payload
) {
    UTILS_DIE_IF(i_tile >= getNTilesInRow(), "i_tile is greater than ntiles_in_row");
    UTILS_DIE_IF(j_tile >= getNTilesInCol(), "j_tile is greater than ntiles_in_col");
    UTILS_DIE_IF(i_tile > j_tile && isIntraSCM(), "Accessing lower triangle of intra SCM is not allowed!");

    tile_payloads[i_tile][j_tile] = std::move(tile_payload);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixPayload::getNTilesInRow() const {
    return tile_payloads.size();
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixPayload::getNTilesInCol() const {
    if (getNTilesInRow() > 0)
        return tile_payloads[0].size();
    else
        return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixPayload::isIntraSCM() const{
    return chr1_ID == chr2_ID;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixPayload::getSize() const{
    size_t size = 1u; // parameter_set_ID
    size += 1u; // sample_ID
    size += 1u; // chr1_ID
    size += 1u; // chr2_ID

    auto ntiles_in_row = getNTilesInRow();
    auto ntiles_in_col = getNTilesInCol();
    for (auto i = 0u; i< ntiles_in_row; i++){
        for (auto j = 0u; j< ntiles_in_col; j++){
            if (!(isIntraSCM() && i>j)){
                size += TILE_PAYLOAD_SIZE_LEN;
                size += tile_payloads[i][j].getSize();
            }
        }
    }

    // TODO (Yeremia): Missing norm_matrices

    if (row_mask_payload.has_value()) {
        size += MASK_PAYLOAD_SIZE_LEN;
        size += row_mask_payload->getSize();
    }

    if (!isIntraSCM() && col_mask_payload.has_value()) {
        size += MASK_PAYLOAD_SIZE_LEN;
        size += col_mask_payload->getSize();
    }

    return size;
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::write(util::BitWriter &writer) const{
    writer.writeBypassBE(parameter_set_ID);
    writer.writeBypassBE(sample_ID);
    writer.writeBypassBE(chr1_ID);
    writer.writeBypassBE(chr2_ID);

    for (auto i = 0u; i<getNTilesInRow(); i++){
        for (auto j = 0u; j<getNTilesInCol(); j++){
            if (!(isIntraSCM() && i>j)){
                auto& tile_payload = tile_payloads[i][j];
                auto tile_payload_size = tile_payload.getSize();

                writer.writeBypassBE(
                    static_cast<uint32_t>(tile_payload_size)
                );

                tile_payload.write(writer);
            }
        }
    }

    // TODO (Yeremia): Missing norm_matrices
    for (auto& v: norm_tile_payloads){

    }

    if (row_mask_payload.has_value()){
        auto row_mask_payload_size = row_mask_payload->getSize();
        writer.writeBypassBE(
            static_cast<uint32_t>(row_mask_payload_size)
        );
        row_mask_payload->write(writer);
    }

    if (!isIntraSCM() && col_mask_payload.has_value()){
        auto col_mask_payload_size = col_mask_payload->getSize();
        writer.writeBypassBE(
            static_cast<uint32_t>(col_mask_payload_size)
        );
        col_mask_payload->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}
}

// ---------------------------------------------------------------------------------------------------------------------
