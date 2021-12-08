/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_PARAMETERS_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_PARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief
 */
class BinarizationParameters {
 public:
    /**
     * @brief
     */
    enum class BinarizationId : uint8_t {
        BI = 0,
        TU = 1,
        EG = 2,
        SEG = 3,
        TEG = 4,
        STEG = 5,
        SUTU = 6,
        SSUTU = 7,
        DTU = 8,
        SDTU = 9,
    };

    /**
     * @brief
     */
    BinarizationParameters();

    /**
     * @brief
     * @param binID
     * @param reader
     */
    BinarizationParameters(BinarizationId binID, util::BitReader &reader);

    /**
     * @brief
     * @param _binarization_id
     * @param params
     */
    BinarizationParameters(const BinarizationId &_binarization_id, std::vector<uint8_t> params);

    /**
     *
     */
    virtual ~BinarizationParameters() = default;

    /**
     * @brief
     * @param binID
     * @param writer
     */
    virtual void write(BinarizationId binID, util::BitWriter &writer) const;

    /**
     * @brief
     * @return
     */
    uint8_t getCMax() const;

    /**
     * @brief
     * @return
     */
    uint8_t getCMaxTeg() const;

    /**
     * @brief
     * @return
     */
    uint8_t getCMaxDtu() const;

    /**
     * @brief
     * @return
     */
    uint8_t getSplitUnitSize() const;

    /**
     * @brief
     * @param binarzationId
     * @return
     */
    static uint8_t getNumBinarizationParams(BinarizationParameters::BinarizationId binarzationId);

    bool operator==(const BinarizationParameters &bin) const {
        return cmax == bin.cmax && cmax_teg == bin.cmax_teg && cmax_dtu == bin.cmax_dtu &&
               split_unit_size == bin.split_unit_size;
    }

 private:
    uint8_t cmax;             //!< @brief
    uint8_t cmax_teg;         //!< @brief
    uint8_t cmax_dtu;         //!< @brief
    uint8_t split_unit_size;  //!< @brief

    static uint8_t numParams[unsigned(BinarizationId::SDTU) + 1u];  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_PARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
