/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/


#ifndef GENIE_CONTACT_CONTACT_MATRIX_TILE_PAYLOAD_H
#define GENIE_CONTACT_CONTACT_MATRIX_TILE_PAYLOAD_H

#include <cstdint>
#include <list>
#include <optional>
#include <tuple>
#include <xtensor/xarray.hpp>
#include <xtensor/xtensor.hpp>
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::contact {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @class ContactMatrixTilePayload
 *
 * @brief This class represents a payload_ for a contact matrix tile.
 *
 * This class encapsulates the payload_ for a contact matrix tile.
 * It includes the codec ID, the number of rows and columns in the tile, and the payload_ itself.
 */
class ContactMatrixTilePayload {
  private:
    core::AlgoID codec_ID_;
    uint32_t tile_nrows_;
    uint32_t tile_ncols_;
    std::vector<uint8_t> payload_;

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
    explicit ContactMatrixTilePayload(util::BitReader &reader, size_t payload_size);

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
     * @param payload The payload_.
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
     * @param payload The address of the address of the payload_.
     * @param payload_len The length of the payload_.
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
    [[nodiscard]] core::AlgoID GetCodecID() const;

    /**
     * @brief Get the number of rows in the tile.
     *
     * This function returns the number of rows in the tile.
     *
     * @return The number of rows in the tile.
     */
    [[nodiscard]] uint32_t GetTileNRows() const;

    /**
     * @brief Get the number of columns in the tile.
     *
     * This function returns the number of columns in the tile.
     *
     * @return The number of columns in the tile.
     */
    [[nodiscard]] uint32_t GetTileNCols() const;

    /**
     * @brief Get the payload_.
     *
     * This function returns the payload_.
     *
     * @return The payload_.
     */
    [[nodiscard]] const std::vector<uint8_t>& GetPayload() const;

    /**
     * @brief Set the Codec ID.
     *
     * This function sets the Codec ID.
     *
     * @param id The new Codec ID.
     */
    [[maybe_unused]] void SetCodecID(core::AlgoID id);

    /**
     * @brief Set the number of rows in the tile.
     *
     * This function sets the number of rows in the tile.
     *
     * @param rows The new number of rows.
     */
    [[maybe_unused]] void SetTileNRows(uint32_t rows);

    /**
     * @brief Set the number of columns in the tile.
     *
     * This function sets the number of columns in the tile.
     *
     * @param cols The new number of columns.
     */
    [[maybe_unused]] void SetTileNCols(uint32_t cols);

    /**
     * @brief Set the payload_.
     *
     * This function sets the payload_.
     *
     * @param data The new payload_.
     */
    [[maybe_unused]] void SetPayload(const std::vector<uint8_t>& data);

    /**
     * @brief Gets the size of the payload_.
     *
     * This function returns the size of the payload_.
     *
     * @return The size of the payload_.
     */
    [[nodiscard]] size_t GetPayloadSize() const;

    /**
     * @brief Gets the size of this structure.
     *
     * This function returns the size of this structure.
     *
     * @return The size of the payload_.
     */
    [[nodiscard]] size_t GetSize() const;

    /**
     * @brief Writes the object to a writer.
     *
     * This function writes the object to a writer.
     *
     * @param writer The writer to write to.
     */
    void Write(util::BitWriter &writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

} // namespace genie::contact

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONTACT_MATRIX_TILE_PAYLOAD_H
