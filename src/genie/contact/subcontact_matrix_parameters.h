/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/
#ifndef GENIE_CONTACT_SUBCONTACT_MATRIX_PARAMETERS_H
#define GENIE_CONTACT_SUBCONTACT_MATRIX_PARAMETERS_H

#include <genie/core/constants.h>
#include <genie/core/writer.h>
#include <genie/util/bit_reader.h>
#include <genie/util/bit_writer.h>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "contact_matrix_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

#define ENA_TRANSFORM_BLEN 1
#define CODEC_ID_BLEN 5

// ---------------------------------------------------------------------------------------------------------------------

enum class DiagonalTransformMode : uint8_t {
    NONE = 0,
    MODE_0 = 1,
    MODE_1 = 2,
    MODE_2 = 3,
    MODE_3 = 4
};

// ---------------------------------------------------------------------------------------------------------------------

enum class BinarizationMode : uint8_t {
    NONE = 0,
    ROW_BINARIZATION = 1
};

// ---------------------------------------------------------------------------------------------------------------------

struct TileParameter {
    DiagonalTransformMode diag_tranform_mode;
    BinarizationMode binarization_mode;
};

// ---------------------------------------------------------------------------------------------------------------------

//using ChrIDPair = std::pair<uint8_t, uint8_t>;
using TileParameters = std::vector<std::vector<TileParameter>>;

// ---------------------------------------------------------------------------------------------------------------------

class SubcontactMatrixParameters {
 private:
    uint8_t parameter_set_ID;
    uint8_t chr1_ID;
    uint8_t chr2_ID;
    core::AlgoID codec_ID;
    TileParameters tile_parameters;
    bool row_mask_exists_flag;
    bool col_mask_exists_flag;

 public:
    // Default constructor
    SubcontactMatrixParameters();

    // Constructor from move
    SubcontactMatrixParameters(
        SubcontactMatrixParameters&& other
    ) noexcept;

    // Constructor by reference
    SubcontactMatrixParameters(
        const SubcontactMatrixParameters& other
    ) noexcept;

    // Constructor from move
//    SubcontactMatrixParameters(
//        uint8_t parameter_set_ID,
//        uint8_t chr1_ID,
//        uint8_t chr2_ID,
//        core::AlgoID codec_ID,
//        TileParameters&& tile_parameters,
//        bool row_mask_exists_flag,
//        bool col_mask_exists_flag
//    ) noexcept;

    SubcontactMatrixParameters(
        util::BitReader& reader,
        ContactMatrixParameters& cm_params);

    // Constructor using operator=
    SubcontactMatrixParameters& operator=(
        const SubcontactMatrixParameters& other
    );

    bool operator==(
        const SubcontactMatrixParameters& other
    );

    /**
     * @brief Get the parameter set ID.
     *
     * This method returns the parameter set ID.
     *
     * @return The parameter set ID.
     */
    uint8_t getParameterSetID() const;

    /**
     * @brief Get the first chromosome ID.
     *
     * This method returns the first chromosome ID.
     *
     * @return The first chromosome ID.
     */
    uint8_t getChr1ID() const;

    /**
     * @brief Get the second chromosome ID.
     *
     * This method returns the second chromosome ID.
     *
     * @return The second chromosome ID.
     */
    uint8_t getChr2ID() const;

    core::AlgoID getCodecID() const;

//    const std::vector<std::vector<TileParameter>>& getTileParameters() const;
     /**
     * returns a constant reference to the tile parameters.
     *
     * @return A constant reference to TileParameter.
     */
    const TileParameters& getTileParameters() const;

    /**
     * @brief Get the row mask exists flag.
     *
     * This method returns the row mask exists flag.
     *
     * @return The row mask exists flag.
     */
    bool getRowMaskExistsFlag() const;

    /**
     * @brief Get the column mask exists flag.
     *
     * This method returns the column mask exists flag.
     *
     * @return The column mask exists flag.
     */
    bool getColMaskExistsFlag() const;

    // Setters
    /**
     * @brief Set the parameter set ID.
     *
     * This method sets the parameter set ID.
     *
     * @param id The new parameter set ID.
     */
    void setParameterSetID(uint8_t ID);

    /**
     * @brief Set the first chromosome ID.
     *
     * This method sets the first chromosome ID.
     *
     * @param id The new first chromosome ID.
     */
    void setChr1ID(uint8_t ID);

    /**
     * @brief Set the second chromosome ID.
     *
     * This method sets the second chromosome ID.
     *
     * @param id The new second chromosome ID.
     */
    void setChr2ID(uint8_t ID);

    void setCodecID(core::AlgoID codec_ID);

    /**
     * @brief Set the tile parameters.
     *
     * This method sets the tile parameters.
     *
     * @param parameters The new tile parameters.
     */
    void setTileParameters(TileParameters&& parameters);

    /**
     * @brief Set the row mask exists flag.
     *
     * This method sets the row mask exists flag.
     *
     *  @param flag The new row mask exists flag.
     */
    void setRowMaskExistsFlag(bool flag);

    /**
     * @brief Set the column mask exists flag.
     *
     * This method sets the column mask exists flag.
     *
     *  @param flag The new column mask exists flag.
     */
    void setColMaskExistsFlag(bool flag);

    /**
    * @brief Sets the number of tiles in the subcontact matrix parameters and resizes the internal storage accordingly.
    *
    * @param ntiles_in_row The new number of tiles in a row.
    * @param ntiles_in_col The new number of tiles in a column.
    * @param free_mem If true, frees the memory allocated for the current tile parameters before resizing.
    */
    void setNumTiles(
        size_t ntiles_in_row,
        size_t ntiles_in_col,
        bool free_mem=true
    );

    /**
    * @brief Retrieves a tile parameter from the subcontact matrix parameters.
    *
    * @param i_tile The row index of the tile parameter.
    * @param j_tile The column index of the tile parameter.
    * @return A reference to the tile parameter at the specified indices.
    * @throws std::runtime_error If i_tile is out of bounds (i.e., i_tile >= getNTilesInRow()) or j_tile is out of bounds (i.e., j_tile >= getNTilesInCol()).
    * @throws std::runtime_error If i_tile > j_tile and isIntraSCM() is true, indicating an attempt to access the lower triangle of an intra SCM, which is not allowed.
    */
    TileParameter& getTileParameter(
        size_t i_tile,
        size_t j_tile
    );

    /**
     * This method sets the tile parameter for the tile in the position i and j.
     *
     * @param i_tile index of the tile in the row
     * @param j_tile index of the tile in the column
     * @param tile_parameter
     */
    void setTileParameter(
        size_t i_tile,
        size_t j_tile,
        TileParameter tile_parameter
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
     * @brief Check if the subcontact matrix is symmetrical.
     *
     * This method checks if the subcontact matrix is symmetrical.
     * It returns true if the chromosome IDs are the same, and false otherwise.
     *
     * @return True if the subcontact matrix is symmetrical, false otherwise.
     */
    bool isIntraSCM() const;

    /**
     * @brief Computes the size of the object in bytes.
     *
     * This function computes the size of the object in bytes.
     * It adds the size of each member variable to the total size.
     *
     * @return The size of the object in bytes.
     */
    size_t getSize() const;

    /**
     * @brief Writes the object to a BitWriter.
     *
     * This function writes the object to a BitWriter.
     * It writes each member variable to the BitWriter.
     *
     * @param writer The BitWriter to write to.
     */
    void write(util::BitWriter& writer) const;
    void write(core::Writer& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_SUBCONTACT_MATRIX_PARAMETERS_H
