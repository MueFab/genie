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
#include "contact_matrix_parameters.h"
#include "subcontact_matrix_parameters.h"
#include "contact_matrix_tile_payload.h"
#include "subcontact_matrix_mask_payload.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

#define TILE_PAYLOAD_SIZE_LEN 4u;
#define MASK_PAYLOAD_SIZE_LEN 4u;

// ---------------------------------------------------------------------------------------------------------------------

//using TilePayloads = xt::xtensor<ContactMatrixTilePayload, 2, xt::layout_type::row_major>;
using TilePayloads = std::vector<std::vector<ContactMatrixTilePayload>>;
using NormTilePayloads = std::vector<std::vector<uint8_t>>;

// ---------------------------------------------------------------------------------------------------------------------

class SubcontactMatrixPayload {
  private:
      uint8_t parameter_set_ID;
      uint8_t sample_ID;
      uint8_t chr1_ID;
      uint8_t chr2_ID;
      TilePayloads tile_payloads;
      std::vector<NormTilePayloads> norm_tile_payloads;
      std::optional<SubcontactMatrixMaskPayload> row_mask_payload;
      std::optional<SubcontactMatrixMaskPayload> col_mask_payload;

  public:
    /**
     * @brief Default constructor for SubcontactMatrixPayload.
     *
     * This is the default constructor for the SubcontactMatrixPayload class. It initializes an empty object.
     */
    SubcontactMatrixPayload();

    /**
     * @brief Constructor from move.
     *
     * This constructor moves the contents of another SubcontactMatrixPayload object into this one.
     *
     * @param other The SubcontactMatrixPayload object to move from.
     */
    SubcontactMatrixPayload(SubcontactMatrixPayload&& other) = default;

    /**
     * @brief Constructor by reference.
     *
     * This constructor copies the contents of another SubcontactMatrixPayload object into this one.
     *
     * @param other The SubcontactMatrixPayload object to copy from.
     */
    SubcontactMatrixPayload(const SubcontactMatrixPayload& other) = default;

    /**
     * @brief Constructor using operator=.
     *
     * This constructor assigns the contents of another SubcontactMatrixPayload object to this one.
     *
     * @param other The SubcontactMatrixPayload object to assign from.
     */
    SubcontactMatrixPayload& operator=(
        const SubcontactMatrixPayload& other
    ) = default;



    /**
     * @brief Parameterized constructor for SubcontactMatrixPayload.
     *
     * This is the parameterized constructor for the SubcontactMatrixPayload class. It initializes an object with the given parameters.
     *
     * @param parameter_set_ID The parameter set ID.
     * @param sample_ID The sample ID.
     * @param chr1_ID The ID of the first chromosome.
     * @param chr2_ID The ID of the second chromosome.
     */
    SubcontactMatrixPayload(
        uint8_t parameter_set_ID,
        uint8_t sample_ID,
        uint8_t chr1_ID,
        uint8_t chr2_ID
    );

    /**
     * @brief Parameterized constructor for SubcontactMatrixPayload.
     *
     * This is the parameterized constructor for the SubcontactMatrixPayload class. It initializes an object with the given parameters.
     *
     * @param parameter_set_ID The parameter set ID.
     * @param sample_ID The sample ID.
     * @param chr1_ID The ID of the first chromosome.
     * @param chr2_ID The ID of the second chromosome.
     * @param tile_payloads The tile payloads.
     * @param row_mask_payload The row mask payload.
     * @param col_mask_payload The column mask payload.
     */
    SubcontactMatrixPayload(
        uint8_t parameter_set_ID,
        uint8_t sample_ID,
        uint8_t chr1_ID,
        uint8_t chr2_ID,
        TilePayloads&& tile_payloads,
        std::optional<SubcontactMatrixMaskPayload>&& row_mask_payload,
        std::optional<SubcontactMatrixMaskPayload>&& col_mask_payload
    );

    //TODO(yeremia): docstring
    SubcontactMatrixPayload(
        util::BitReader &reader,
        ContactMatrixParameters& cm_param,
        const SubcontactMatrixParameters& scm_param
    ) noexcept;

    //TODO(yeremia): docstring
    bool operator==(
        SubcontactMatrixPayload& other
    );

//    friend bool operator==(const SubcontactMatrixPayload& lhs, const SubcontactMatrixPayload& rhs);

   /**
    * @brief Get the Parameter Set ID.
    *
    * This function returns the ID of the parameter set.
    *
    * @return The ID of the parameter set.
    */
   uint8_t getParameterSetID() const;

   /**
    * @brief Get the Sample ID.
    *
    * This function returns the ID of the sample.
    *
    * @return The ID of the sample.
    */
   uint8_t getSampleID() const;

   /**
    * @brief Get the Chromosome 1 ID.
    *
    * This function returns the ID of the first chromosome.
    *
    * @return The ID of the first chromosome.
    */
   uint8_t getChr1ID() const;

   /**
    * @brief Get the Chromosome 2 ID.
    *
    * This function returns the ID of the second chromosome.
    *
    * @return The ID of the second chromosome.
    */
   uint8_t getChr2ID() const;

   /**
    * @brief Gets the tile payloads.
    *
    * This function returns a constant reference to the tile payloads. The payloads are of type `TilePayloads`.
    *
    * @return A constant reference to the tile payloads.
    */
   const TilePayloads& getTilePayloads() const;

   //TODO(yeremia): docstring
   const size_t getNumNormMatrices() const;

   //TODO(yeremia): docstring
   bool anyRowMaskPayload() const;

   //TODO(yeremia): docstring
   const SubcontactMatrixMaskPayload& getRowMaskPayload() const;

   //TODO(yeremia): docstring
   bool anyColMaskPayload() const;

   //TODO(yeremia): docstring
   const SubcontactMatrixMaskPayload& getColMaskPayload() const;

   /**
    * @brief Set the Parameter Set ID.
    *
    * This function sets the ID of the parameter set.
    *
    * @param id The ID of the parameter set.
    */
   void setParameterSetID(uint8_t id);

   /**
    * @brief Set the Sample ID.
    *
    * This function sets the ID of the sample.
    *
    * @param id The ID of the sample.
    */
   void setSampleID(uint8_t id);

   /**
    * @brief Set the Chromosome 1 ID.
    *
    * This function sets the ID of the first chromosome.
    *
    * @param id The ID of the first chromosome.
    */
   void setChr1ID(uint8_t id);

   /**
    * @brief Set the Chromosome 2 ID.
    *
    * This function sets the ID of the second chromosome.
    *
    * @param id The ID of the second chromosome.
    */
   void setChr2ID(uint8_t id);

   /**
    * @brief Set the Tile Payloads.
    *
    * This function sets the Tile Payloads.
    *
    *  @param payloads The new Tile Payloads.
    */
   void setTilePayloads(const TilePayloads& payloads);

   /**
    * @brief Sets the payload for the row mask.
    *
    * This function sets the payload for the row mask. The payload is an optional object of type `SubcontactMatrixMaskPayload`.
    *
    * @param payload The payload to set for the row mask.
    */
   void setRowMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload);

//   void setRowMaskPayload(SubcontactMatrixMaskPayload&& payload);

   /**
    * @brief Sets the payload for the column mask.
    *
    * This function sets the payload for the column mask. The payload is an optional object of type `SubcontactMatrixMaskPayload`.
    *
    * @param payload The payload to set for the column mask.
    */
   void setColMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload);

//   void setColMaskPayload(SubcontactMatrixMaskPayload&& payload);

   //TODO(yeremia): docstring
   void setNumTiles(
       size_t ntiles_in_row,
       size_t ntiles_in_col,
       bool free_mem=true
   );

   ContactMatrixTilePayload& getTilePayload(
       size_t i_tile,
       size_t j_tile
   );

   /**
     * @brief Adds a tile payload.
     *
     * This method adds a tile payload at the given indices.
     *
     * @param i_tile The index of the first dimension.
     * @param j_tile The index of the second dimension.
     * @param tile_payload The tile payload to set.
    */
   void setTilePayload(
       size_t i_tile,
       size_t j_tile,
       ContactMatrixTilePayload&& tile_payload
   );

   /**
     * @brief Get the number of tiles in a row.
     *
     * This method returns the number of tiles in a row.
     *
     * @return The number of tiles in a row.
    */
   size_t getNTilesInRow() const;

   /**
     * @brief Get the number of tiles in a column.
     *
     * This method returns the number of tiles in a column.
     *
     * @return The number of tiles in a column.
    */
   size_t getNTilesInCol() const;

   /**
    * @brief Checks if the subcontact matrix matrix is intra subcontact matrix.
    *
    * This method checks if the subcontact matrix is intra.
    * It returns true if the chromosome IDs are the same, and false otherwise.
    *
    * @return True if the subcontact matrix is intra, false otherwise.
    */
   bool isIntraSCM() const;

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

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_SUBCONTACT_MATRIX_PAYLOAD_H
