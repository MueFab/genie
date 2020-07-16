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
 *
 */
class BinarizationParameters {
   public:
    /**
     *
     */
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

    /**
     *
     */
    BinarizationParameters();

    /**
     *
     * @param binID
     * @param reader
     */
    BinarizationParameters(BinarizationId binID, util::BitReader &reader);

    /**
     *
     * @param _binarization_id
     * @param params
     */
    BinarizationParameters(const BinarizationId &_binarization_id, std::vector<uint8_t> params);

    /**
     *
     */
    virtual ~BinarizationParameters() = default;

    /**
     *
     * @param binID
     * @param writer
     */
    virtual void write(BinarizationId binID, util::BitWriter &writer) const;

    /**
     *
     * @return
     */
    uint8_t getCMax() const;

    /**
     *
     * @return
     */
    uint8_t getCMaxTeg() const;

    /**
     *
     * @return
     */
    uint8_t getCMaxDtu() const;

    /**
     *
     * @return
     */
    uint8_t getSplitUnitSize() const;

    /**
     *
     * @param binarzationId
     * @return
     */
    static uint8_t getNumBinarizationParams(BinarizationParameters::BinarizationId binarzationId);

    bool operator==(const BinarizationParameters& bin) const {
        return cmax == bin.cmax && cmax_teg == bin.cmax_teg && cmax_dtu == bin.cmax_dtu && split_unit_size == bin.split_unit_size;
    }

   private:
    uint8_t cmax;             //!<
    uint8_t cmax_teg;         //!<
    uint8_t cmax_dtu;         //!<
    uint8_t split_unit_size;  //!<

    static uint8_t numParams[unsigned(BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY) + 1u];  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BINARIZATION_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
