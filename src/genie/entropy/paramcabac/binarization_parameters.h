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
 *
 */
class BinarizationParameters {
 public:
    /**
     *
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

    bool operator==(const BinarizationParameters &bin) const {
        return cmax == bin.cmax && cmax_teg == bin.cmax_teg && cmax_dtu == bin.cmax_dtu &&
               split_unit_size == bin.split_unit_size;
    }

 private:
    uint8_t cmax;             //!<
    uint8_t cmax_teg;         //!<
    uint8_t cmax_dtu;         //!<
    uint8_t split_unit_size;  //!<

    static uint8_t numParams[unsigned(BinarizationId::SDTU) + 1u];  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_PARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
