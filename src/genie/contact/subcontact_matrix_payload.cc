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

const TilePayloadsDtype& SubcontactMatrixPayload::getTilePayloads() const { return tile_payloads; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixPayload::setTilePayloads(const TilePayloadsDtype& payloads) { tile_payloads = payloads; }

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
