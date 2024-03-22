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
    uint8_t _chr2_ID,
    TilePayloads&& _tile_payloads,
    std::optional<SubcontactMatrixMaskPayload>&& _row_mask_payload,
    std::optional<SubcontactMatrixMaskPayload>&& _col_mask_payload
    ) : parameter_set_ID(_parameter_set_ID),
        sample_ID(_sample_ID),
        chr1_ID(_chr1_ID),
        chr2_ID(_chr2_ID),
        tile_payloads(std::move(_tile_payloads)),
        row_mask_payload(std::move(_row_mask_payload)),
        col_mask_payload(std::move(_col_mask_payload)) {}


// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixPayload::getParameterSetID() const { return parameter_set_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setParameterSetID(uint8_t id) { parameter_set_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixPayload::getSampleID() const { return sample_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setSampleID(uint8_t id) { sample_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixPayload::getChr1ID() const { return chr1_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setChr1ID(uint8_t id) { chr1_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SubcontactMatrixPayload::getChr2ID() const { return chr2_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setChr2ID(uint8_t id) { chr2_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

const TilePayloads& SubcontactMatrixPayload::getTilePayloads() const { return tile_payloads; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setTilePayloads(const TilePayloads& payloads) { tile_payloads = payloads; }

// ---------------------------------------------------------------------------------------------------------------------

const std::optional<SubcontactMatrixMaskPayload>& SubcontactMatrixPayload::getRowMaskPayload() const { return row_mask_payload; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setRowMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload) { row_mask_payload = payload; }

// ---------------------------------------------------------------------------------------------------------------------

const std::optional<SubcontactMatrixMaskPayload>& SubcontactMatrixPayload::getColMaskPayload() const { return col_mask_payload; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setColMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload) { col_mask_payload = payload; }


// ---------------------------------------------------------------------------------------------------------------------

}
}

// ---------------------------------------------------------------------------------------------------------------------
