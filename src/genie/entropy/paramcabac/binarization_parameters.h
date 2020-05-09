/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_BINARIZATION_PARAMETERS_H
#define GENIE_BINARIZATION_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * ISO 23092-2 Section 8.3.3.1 table 98
 */
class BinarizationParameters {
   public:
    enum class BinarizationId : uint8_t {
        BINARY_CODING = 0,
        TRUNCATED_UNARY = 1,
        EXPONENTIAL_GOLOMB = 2,
        SIGNED_EXPONENTIAL_GOMB = 3,
        TRUNCATED_EXPONENTIAL_GOLOMB = 4,
        SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB = 5,
        SPLIT_UNITWISE_TRUNCATED_UNARY = 6,
        SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY = 7,
        DOUBLE_TRUNCATED_UNARY = 8,
        SIGNED_DOUBLE_TRUNCATED_UNARY = 9,
    };

    BinarizationParameters();

    BinarizationParameters(BinarizationId binID, util::BitReader &reader);

    BinarizationParameters(const BinarizationId &_binarization_id, std::vector<uint8_t> params);

    virtual ~BinarizationParameters() = default;

    virtual void write(BinarizationId binID, util::BitWriter &writer) const;

    uint8_t getCMax() const;

    uint8_t getCMaxTeg() const;

    uint8_t getCMaxDtu() const;

    uint8_t getSplitUnitSize() const;

    static uint8_t getNumBinarizationParams(BinarizationParameters::BinarizationId binarzationId);

   private:
    uint8_t cmax;             //!< : 8; Line 3
    uint8_t cmax_teg;         //!< : 8; Line 5
    uint8_t cmax_dtu;         //!< : 8; Line 7
    uint8_t split_unit_size;  //!< : 4; Line 10
    
    static uint8_t numParams[unsigned(BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY) + 1u];

};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BINARIZATION_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
