/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include "subcontact_matrix_payload.h"

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

//bool SubcontactMatrixPayload::operator==(const SubcontactMatrixPayload& other) const {
//    return parameter_set_ID == other.parameter_set_ID &&
//           sample_ID == other.sample_ID &&
//           chr1_ID == other.chr1_ID &&
//           chr2_ID == other.chr2_ID &&
//           tile_payloads == other.tile_payloads &&
//           row_mask_payload == other.row_mask_payload &&
//           col_mask_payload == other.col_mask_payload;
//}

// ---------------------------------------------------------------------------------------------------------------------

//friend bool operator==(const SubcontactMatrixPayload& lhs, const SubcontactMatrixPayload& rhs){
//
//}

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

void SubcontactMatrixPayload::setRowMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload) { row_mask_payload = payload; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setColMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload) { col_mask_payload = payload; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::addTilePayload(
    size_t i_tile,
    size_t j_tile,
    ContactMatrixTilePayload&& tile_payload
) {
    // Assuming TilePayloads is a 2D vector of ContactMatrixTilePayload
    if (i_tile >= tile_payloads.size()) {
        tile_payloads.resize(i_tile + 1);
    }
    if (j_tile >= tile_payloads[i_tile].size()) {
        tile_payloads[i_tile].resize(j_tile + 1);
    }
    tile_payloads[i_tile][j_tile] = std::move(tile_payload);
}

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixPayload::isIntraSCM() const{
    return chr1_ID == chr2_ID;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixPayload::getSize() const{
    size_t size = 1 + 1 + 1 + 1; // parameter_sert_ID, sample_ID, chr1_ID, chr2_ID
    auto nrows = tile_payloads.size();
    auto ncols = tile_payloads[0].size();
    for (auto i = 0u; i<nrows; i++){
        for (auto j = 0u; j<ncols; j++){
            if (i>j && isIntraSCM()){
                continue;
            }
            size += TILE_PAYLOAD_SIZE_LEN;
            size += tile_payloads[i][j].getSize();
        }
    }

    // TODO (Yeremia): Missing norm_matrices

    if (row_mask_payload.has_value())
        size += MASK_PAYLOAD_SIZE_LEN;
        size += row_mask_payload->getSize();

    if (col_mask_payload.has_value())
        size += MASK_PAYLOAD_SIZE_LEN;
        size += col_mask_payload->getSize();

    return size;
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::write(util::BitWriter &writer) const{
    writer.writeBypassBE(parameter_set_ID);
    writer.writeBypassBE(sample_ID);
    writer.writeBypassBE(chr1_ID);
    writer.writeBypassBE(chr2_ID);

    auto nrows = tile_payloads.size();
    auto ncols = tile_payloads[0].size();
    for (auto i = 0u; i<nrows; i++){
        for (auto j = 0u; j<ncols; j++){
            if (i>j && isIntraSCM()){
                continue;
            }

            writer.writeBypassBE(
                static_cast<uint32_t>(tile_payloads[i][j].getSize())
            );
            tile_payloads[i][j].write(writer);
        }
    }

    // TODO (Yeremia): Missing norm_matrices

    if (row_mask_payload.has_value()){
        writer.writeBypassBE(
            static_cast<uint32_t>(row_mask_payload->getSize())
        );
        row_mask_payload->write(writer);
    }

    if (col_mask_payload.has_value()){
        writer.writeBypassBE(
            static_cast<uint32_t>(col_mask_payload->getSize())
        );
        col_mask_payload->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}
}

// ---------------------------------------------------------------------------------------------------------------------
