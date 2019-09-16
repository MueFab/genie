#ifndef GENIE_CABAC_BINARIZATION_PARAMETERS_H
#define GENIE_CABAC_BINARIZATION_PARAMETERS_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>

// -----------------------------------------------------------------------------------------------------------------

namespace format {
class BitWriter;
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

    virtual void write(BitWriter *writer) const;

    CabacBinarizationParameters();

    CabacBinarizationParameters(const BinarizationId &_binarization_id, uint8_t param);

    std::unique_ptr<CabacBinarizationParameters> clone() const;
};
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CABAC_BINARIZATION_PARAMETERS_H
