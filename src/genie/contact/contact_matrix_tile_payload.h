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

/**
 * @class ContactMatrixTilePayload
 *
 * @brief This class represents a payload for a contact matrix tile.
 *
 * This class encapsulates the payload for a contact matrix tile.
 * It includes the codec ID, the number of rows and columns in the tile, and the payload itself.
 */
class ContactMatrixTilePayload {
  private:
    core::AlgoID codec_ID;
    uint32_t tile_nrows;
    uint32_t tile_ncols;
    std::vector<uint8_t> payload;

  public:

    /**
     * @brief Constructor for ContactMatrixTilePayload.
     *
     * This is the parameterized constructor for the ContactMatrixTilePayload class.
     * It initializes an object with the given parameters.
     *
     * @param codec_ID The codec ID.
     * @param tile_nrows The number of rows in the tile.
     * @param tile_ncols The number of columns in the tile.
     * @param payload The payload.
     */
    ContactMatrixTilePayload(
        core::AlgoID codec_ID,
        uint32_t tile_nrows,
        uint32_t tile_ncols,
        std::vector<uint8_t>&& payload
    );

    /**
     * @brief Constructor for ContactMatrixTilePayload from a 2D array.
     *
     * This constructor initializes an object from a 2D array.
     *
     * @param codec_ID The codec ID.
     * @param tile_nrows The number of rows in the tile.
     * @param tile_ncols The number of columns in the tile.
     * @param payload The address of the address of the payload.
     * @param payload_len The length of the payload.
     */
    ContactMatrixTilePayload(
        core::AlgoID codec_ID,
        uint32_t tile_nrows,
        uint32_t tile_ncols,
        uint8_t** payload,
        size_t payload_len
    );

    /**
     * @brief Gets the size of this structure.
     *
     * This function returns the size of this structure.
     *
     * @return The size of the payload.
     */
    size_t getSize() const;

    /**
     * @brief Gets the size of the payload.
     *
     * This function returns the size of the payload.
     *
     * @return The size of the payload.
     */
    size_t getPayloadSize() const;

    /**
     * @brief Writes the object to a writer.
     *
     * This function writes the object to a writer.
     *
     * @param writer The writer to write to.
     */
    void write(core::Writer& writer) const;
};

}
}

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONTACT_MATRIX_TILE_PAYLOAD_H
