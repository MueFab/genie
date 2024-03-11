/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/


#ifndef GENIE_CONTACT_CONTACT_MATRIX_TILE_PAYLOAD_H
#define GENIE_CONTACT_CONTACT_MATRIX_TILE_PAYLOAD_H

#include <cstdint>
#include <list>
#include <tuple>
#include <optional>
#include <xtensor/xarray.hpp>
#include <xtensor/xtensor.hpp>
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

class ContactMatrixTilePayload {
 private:
    core::AlgoID codec_ID;
    uint32_t tile_nrows;
    uint32_t tile_ncols;
    std::vector<uint8_t> payload;

 public:
    size_t getSize() const;
    size_t getPayloadSize() const;

    void write(core::Writer& writer) const;
};

}
}

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONTACT_MATRIX_TILE_PAYLOAD_H
