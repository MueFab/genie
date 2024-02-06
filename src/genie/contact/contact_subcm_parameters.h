/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/
#ifndef GENIE_CONTACT_CONTACT_SUBCM_PARAMETERS_H
#define GENIE_CONTACT_CONTACT_SUBCM_PARAMETERS_H

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
    MODE_0 = 0,
    MODE_1 = 1,
    MODE_2 = 2,
    MODE_3 = 3
};

// ---------------------------------------------------------------------------------------------------------------------

struct TileParameter {
    bool diag_transform_flag;
    DiagonalTransformMode diag_tranform_mode;
    bool binarization_flag;
};

// ---------------------------------------------------------------------------------------------------------------------

using ChrIDPair = std::pair<uint8_t, uint8_t>;

// ---------------------------------------------------------------------------------------------------------------------

class ContactSubmatParameters {
 private:
    uint8_t chr1_ID;
    uint8_t chr2_ID;

    std::vector<std::vector<TileParameter>> tile_parameters;

    bool row_mask_exists_flag;
    bool col_mask_exists_flag;

 public:
    ContactSubmatParameters();

    ContactSubmatParameters(uint8_t chr1_ID, uint8_t chr2_ID, std::vector<std::vector<TileParameter>>&& tile_parameters,
                            bool row_mask_exists_flag, bool col_mask_exists_flag);

//    ContactSubmatParameters(util::BitReader& reader, ContactParameters& param);

    ContactSubmatParameters(util::BitReader& reader, uint8_t chr1_ID, uint8_t chr2_ID, uint32_t ntiles_in_row,
                            uint32_t ntiles_in_col);

    uint8_t getChr1ID() const;
    uint8_t getChr2ID() const;
    ChrIDPair getChrPair();
    const std::vector<std::vector<TileParameter>>& getTileParameters() const;
    bool getRowMaskExistsFlat() const;
    bool getColMaskExistsFlat() const;

    size_t getNTilesInRow() const;
    size_t getNTilesInCol() const;

    void write(core::Writer& writer) const;
    size_t getSize(core::Writer& writesize) const;

};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONTACT_SUBCM_PARAMETERS_H
