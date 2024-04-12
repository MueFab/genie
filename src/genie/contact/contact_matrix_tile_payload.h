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

// ---------------------------------------------------------------------------------------------------------------------

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
      * @brief Default constructor for ContactMatrixTilePayload.
      *
      * This constructor initializes the object with default values.
      */
     ContactMatrixTilePayload();

     /**
      * @brief Constructor using operator=.
      *
      * This constructor assigns the contents of another ContactMatrixTilePayload object to this one.
      *
      * @param other The ContactMatrixTilePayload object to assign from.
      */
     ContactMatrixTilePayload(const ContactMatrixTilePayload& other);

    /**
     * @brief Move constructor for ContactMatrixTilePayload.
     *
     * This is the move constructor for the ContactMatrixTilePayload class.
     * It initializes an object by moving the contents of another object.
     *
     * @param other The other object to move from.
     */
    ContactMatrixTilePayload(ContactMatrixTilePayload&& other) noexcept;

    /**
     * @brief Move assignment operator for ContactMatrixTilePayload.
     *
     * This is the move assignment operator for the ContactMatrixTilePayload class. It assigns the contents of another object to this object by moving it.
     *
     * @param other The other object to move from.
     * @return A reference to this object.
     */
    ContactMatrixTilePayload& operator=(ContactMatrixTilePayload&& other) noexcept;

    /**
     * @brief Constructor that reads from a BitReader.
     *
     * This constructor reads the parameters from a BitReader.
     *
     *  @param reader The BitReader to read from.
     */
    explicit ContactMatrixTilePayload(util::BitReader &reader) noexcept;

    /**
     * @brief Overloaded equality operator.
     *
     * This operator compares two ContactMatrixTilePayload objects for equality.
     * It returns true if all the member variables of the two objects are equal, and false otherwise.
     *
     * @param other The ContactMatrixTilePayload object to compare with.
     * @return True if the two objects are equal, false otherwise.
     */
    bool operator==(const ContactMatrixTilePayload& other) const;

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
     * @brief Get the Codec ID.
     *
     * This function returns the Codec ID.
     *
     * @return The Codec ID.
     */
    core::AlgoID getCodecID() const;

    /**
     * @brief Get the number of rows in the tile.
     *
     * This function returns the number of rows in the tile.
     *
     * @return The number of rows in the tile.
     */
    uint32_t getTileNRows() const;

    /**
     * @brief Get the number of columns in the tile.
     *
     * This function returns the number of columns in the tile.
     *
     * @return The number of columns in the tile.
     */
    uint32_t getTileNCols() const;

    /**
     * @brief Get the payload.
     *
     * This function returns the payload.
     *
     * @return The payload.
     */
    const std::vector<uint8_t>& getPayload() const;

    /**
     * @brief Set the Codec ID.
     *
     * This function sets the Codec ID.
     *
     * @param id The new Codec ID.
     */
    void setCodecID(core::AlgoID id);

    /**
     * @brief Set the number of rows in the tile.
     *
     * This function sets the number of rows in the tile.
     *
     * @param rows The new number of rows.
     */
    void setTileNRows(uint32_t rows);

    /**
     * @brief Set the number of columns in the tile.
     *
     * This function sets the number of columns in the tile.
     *
     * @param cols The new number of columns.
     */
    void setTileNCols(uint32_t cols);

    /**
     * @brief Set the payload.
     *
     * This function sets the payload.
     *
     * @param data The new payload.
     */
    void setPayload(const std::vector<uint8_t>& data);

    /**
     * @brief Gets the size of the payload.
     *
     * This function returns the size of the payload.
     *
     * @return The size of the payload.
     */
    size_t getPayloadSize() const;

    /**
     * @brief Gets the size of this structure.
     *
     * This function returns the size of this structure.
     *
     * @return The size of the payload.
     */
    size_t getSize() const;

    /**
     * @brief Writes the object to a writer.
     *
     * This function writes the object to a writer.
     *
     * @param writer The writer to write to.
     */
    void write(util::BitWriter &writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

} // contact
} // genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONTACT_MATRIX_TILE_PAYLOAD_H
