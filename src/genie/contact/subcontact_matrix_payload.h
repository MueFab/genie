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

using TilePayloads = std::vector<std::vector<ContactMatrixTilePayload>>;

// ---------------------------------------------------------------------------------------------------------------------

class SubcontactMatrixPayload {
  private:
      uint8_t parameter_set_ID;
      uint8_t sample_ID;
      uint8_t chr1_ID;
      uint8_t chr2_ID;
      TilePayloads tile_payloads;
      // TODO (Yeremia): Missing norm_matrices
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

   /**
    * @brief Get the Parameter Set ID.
    *
    * This function returns the ID of the parameter set.
    *
    * @return The ID of the parameter set.
    */
   uint8_t getParameterSetID() const;

   /**
    * @brief Set the Parameter Set ID.
    *
    * This function sets the ID of the parameter set.
    *
    * @param id The ID of the parameter set.
    */
   void setParameterSetID(uint8_t id);

   /**
    * @brief Get the Sample ID.
    *
    * This function returns the ID of the sample.
    *
    * @return The ID of the sample.
    */
   uint8_t getSampleID() const;

   /**
    * @brief Set the Sample ID.
    *
    * This function sets the ID of the sample.
    *
    * @param id The ID of the sample.
    */
   void setSampleID(uint8_t id);

   /**
    * @brief Get the Chromosome 1 ID.
    *
    * This function returns the ID of the first chromosome.
    *
    * @return The ID of the first chromosome.
    */
   uint8_t getChr1ID() const;

   /**
    * @brief Set the Chromosome 1 ID.
    *
    * This function sets the ID of the first chromosome.
    *
    * @param id The ID of the first chromosome.
    */
   void setChr1ID(uint8_t id);

   /**
    * @brief Get the Chromosome 2 ID.
    *
    * This function returns the ID of the second chromosome.
    *
    * @return The ID of the second chromosome.
    */
   uint8_t getChr2ID() const;

   /**
    * @brief Set the Chromosome 2 ID.
    *
    * This function sets the ID of the second chromosome.
    *
    * @param id The ID of the second chromosome.
    */
   void setChr2ID(uint8_t id);

   /**
    * @brief Gets the tile payloads.
    *
    * This function returns a constant reference to the tile payloads. The payloads are of type `TilePayloads`.
    *
    * @return A constant reference to the tile payloads.
    */
   const TilePayloads& getTilePayloads() const;

   /**
     * @brief Adds a tile payload.
     *
     * This method adds a tile payload at the given indices.
     *
     * @param i_tile The index of the first dimension.
     * @param j_tile The index of the second dimension.
     * @param tile_payload The tile payload to set.
    */
   void addTilePayload(size_t i_tile, size_t j_tile, ContactMatrixTilePayload&& tile_payload);

//   void setTilePayloads(TilePayloads&& payloads);

   /**
    * @brief Sets the payload for the row mask.
    *
    * This function sets the payload for the row mask. The payload is an optional object of type `SubcontactMatrixMaskPayload`.
    *
    * @param payload The payload to set for the row mask.
    */
   void setRowMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload);

   /**
    * @brief Gets the payload for the row mask.
    *
    * This function returns the payload for the row mask. It returns a reference to a constant `std::optional` object of type `SubcontactMatrixMaskPayload`.
    *
    * @return A constant reference to the payload for the row mask.
    */
   const std::optional<SubcontactMatrixMaskPayload>& getRowMaskPayload() const;

   /**
    * @brief Sets the payload for the column mask.
    *
    * This function sets the payload for the column mask. The payload is an optional object of type `SubcontactMatrixMaskPayload`.
    *
    * @param payload The payload to set for the column mask.
    */
   void setColMaskPayload(const std::optional<SubcontactMatrixMaskPayload>& payload);

   /**
    * @brief Gets the payload for the column mask.
    *
    * This function returns the payload for the column mask. It returns a reference to a constant `std::optional` object of type `SubcontactMatrixMaskPayload`.
    *
    * @return A constant reference to the payload for the column mask.
    */
   const std::optional<SubcontactMatrixMaskPayload>& getColMaskPayload() const;

   /**
    * @brief Checks if the subcontact matrix matrix is intra subcontact matrix.
    *
    * This method checks if the subcontact matrix is intra.
    * It returns true if the chromosome IDs are the same, and false otherwise.
    *
    * @return True if the subcontact matrix is intra, false otherwise.
    */
   bool isIntraSCM() const;

};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_SUBCONTACT_MATRIX_PAYLOAD_H