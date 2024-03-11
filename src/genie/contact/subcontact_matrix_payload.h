/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#ifndef GENIE_CONTACT_SUBCONTACT_MATRIX_PAYLOAD_H
#define GENIE_CONTACT_SUBCONTACT_MATRIX_PAYLOAD_H

#include <cstdint>
#include <list>
#include <optional>
#include <tuple>
#include <xtensor/xarray.hpp>
#include <xtensor/xtensor.hpp>
#include "genie/core/constants.h"
//#include "genie/core/record/contact/record.h"
#include "contact_parameters.h"
#include "contact_matrix_tile_payload.h"
#include "subcontact_matrix_mask_payload.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

using TilePayloadsDtype = std::vector<std::vector<ContactMatrixTilePayload>>;

// ---------------------------------------------------------------------------------------------------------------------

class SubcontactMatrixPayload {
 private:
    uint8_t parameter_set_ID;
    uint8_t sample_ID;
    uint8_t chr1_ID;
    uint8_t chr2_ID;
    TilePayloadsDtype tile_payloads;
    // TODO (Yeremia): Missing norm_matrices
    std::optional<SubcontactMatrixMaskPayload> row_mask_payload;
    std::optional<SubcontactMatrixMaskPayload> col_mask_payload;

 public:

    uint8_t getParameterSetID() const;
    void setParameterSetID(uint8_t id);
    uint8_t getSampleID() const;
    void setSampleID(uint8_t id);
    uint8_t getChr1ID() const;
    void setChr1ID(uint8_t id);
    void setChr2ID(uint8_t id);
    uint8_t getChr2ID() const;
    const TilePayloadsDtype& getTilePayloads() const;
    void setTilePayloads(const TilePayloadsDtype& payloads);
    void setRowMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload);
    const std::optional<SubcontactMatrixMaskPayload>& getRowMaskPayload() const;
    void setColMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload);
    const std::optional<SubcontactMatrixMaskPayload>& getColMaskPayload() const;


};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_SUBCONTACT_MATRIX_PAYLOAD_H
