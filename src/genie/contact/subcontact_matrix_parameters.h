/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/
#ifndef GENIE_CONTACT_SUBCONTACT_MATRIX_PARAMETERS_H
#define GENIE_CONTACT_SUBCONTACT_MATRIX_PARAMETERS_H

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <genie/core/constants.h>
#include <genie/core/writer.h>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
//#include "contact_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

enum class DiagonalTransformMode : uint8_t {
    NONE = 0,
    MODE_0 = 1,
    MODE_1 = 2,
    MODE_2 = 3,
    MODE_3 = 4
};

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

using ChrIDPair = std::pair<uint8_t, uint8_t>;

// ---------------------------------------------------------------------------------------------------------------------

class SubcontactMatrixParameters {
 private:
    uint8_t parameter_set_ID;
    uint8_t chr1_ID;
    uint8_t chr2_ID;
    std::vector<std::vector<TileParameter>> tile_parameters;
    bool row_mask_exists_flag;
    bool col_mask_exists_flag;

 public:
    SubcontactMatrixParameters();

    SubcontactMatrixParameters(uint8_t parameter_set_ID, uint8_t chr1_ID, uint8_t chr2_ID, std::vector<std::vector<TileParameter>>&& tile_parameters,
                               bool row_mask_exists_flag, bool col_mask_exists_flag);

    SubcontactMatrixParameters(util::BitReader& reader);

    uint8_t getParameterSetID() const;
    void setParameterSetID(uint8_t id);
    uint8_t getChr1ID() const;
    void setChr1ID(uint8_t id);
    uint8_t getChr2ID() const;
    void setChr2ID(uint8_t id);
    ChrIDPair getChrPair();
    bool isSymmetrical() const;
    size_t getNTilesInRow() const;
    size_t getNTilesInCol() const;
    const std::vector<std::vector<TileParameter>>& getTileParameters() const;
    void setTileParameters(const std::vector<std::vector<TileParameter>>& parameters);
    bool getRowMaskExistsFlag() const;
    void setRowMaskExistsFlag(bool flag);
    bool getColMaskExistsFlag() const;
    void setColMaskExistsFlag(bool flag);
    void write(core::Writer& writer) const;
    size_t getSize(core::Writer& writesize) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_SUBCONTACT_MATRIX_PARAMETERS_H
