#ifndef GENIE_CABAC_BINARIZATION_PARAMETERS_H
#define GENIE_CABAC_BINARIZATION_PARAMETERS_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>
#include "util/bitreader.h"
#include "util/bitwriter.h"
#include "util/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_p2 {
namespace desc_conf_pres {
namespace cabac {

// -----------------------------------------------------------------------------------------------------------------

/**
 * ISO 23092-2 Section 8.3.3.1 table 98
 */
class CabacBinarizationParameters {
   private:
    std::unique_ptr<uint8_t> cmax;             //!< : 8; Line 3
    std::unique_ptr<uint8_t> cmax_teg;         //!< : 8; Line 5
    std::unique_ptr<uint8_t> cmax_dtu;         //!< : 8; Line 7
    std::unique_ptr<uint8_t> split_unit_size;  //!< : 4; Line 10
   public:
    enum class BinarizationId : uint8_t {
        BINARY_CODING = 0,
        TRUNCATED_UNARY = 1,
        EXPONENTIAL_GOLOMB = 2,
        SIGNED_EXPONENTIAL_GOMB = 3,
        TRUNCATED_EXPONENTIAL_GOLOMB = 4,
        SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB = 5,
        SPLIT_UNIT_WISE_TRUNCATED_UNARY = 6,
        SIGNED_SPLIT_UNIT_WISE_TRUNCATED_UNARY = 7,
        DOUBLE_TRUNCATED_UNARY = 8,
        SIGNED_DOUBLE_TRUNCATED_UNARY = 9,
    };

    uint8_t getParameter() const {
        if (cmax) {
            return *cmax;
        }
        if (cmax_teg) {
            return *cmax_teg;
        }
        if (cmax_dtu) {
            return *cmax_dtu;
        }
        if (split_unit_size) {
            return *split_unit_size;
        }
        return 0;
    }

    virtual void write(util::BitWriter &writer) const;

    CabacBinarizationParameters();

    CabacBinarizationParameters(BinarizationId binID, util::BitReader &reader) {
        switch (binID) {
            case BinarizationId::TRUNCATED_UNARY:
                cmax = util::make_unique<uint8_t>(reader.read(8));
                break;
            case BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
            case BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
                cmax_teg = util::make_unique<uint8_t>(reader.read(8));
                break;
            case BinarizationId::DOUBLE_TRUNCATED_UNARY:
            case BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
                cmax_dtu = util::make_unique<uint8_t>(reader.read(8));  // Fall-through
            case BinarizationId::SPLIT_UNIT_WISE_TRUNCATED_UNARY:
            case BinarizationId::SIGNED_SPLIT_UNIT_WISE_TRUNCATED_UNARY:
                split_unit_size = util::make_unique<uint8_t>(reader.read(4));
                break;
            default:
                break;
        }
    }

    CabacBinarizationParameters(const BinarizationId &_binarization_id, uint8_t param);

    virtual ~CabacBinarizationParameters() = default;

    std::unique_ptr<CabacBinarizationParameters> clone() const;
};
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CABAC_BINARIZATION_PARAMETERS_H
