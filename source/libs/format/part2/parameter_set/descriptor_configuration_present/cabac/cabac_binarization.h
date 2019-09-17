#ifndef GENIE_CABAC_BINARIZATION_H
#define GENIE_CABAC_BINARIZATION_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>

#include "cabac_binarization_parameters.h"
#include "cabac_context_parameters.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
class BitWriter;
namespace desc_conf_pres {
namespace cabac {

/**
 * ISO 23092-2 Section 8.3.3 table
 */
class CabacBinarization {
   public:
    CabacBinarization(const CabacBinarizationParameters::BinarizationId &_binarization_ID,
                      std::unique_ptr<CabacBinarizationParameters> _cabac_binarization_parameters);

    CabacBinarization();

    void setContextParameters(std::unique_ptr<CabacContextParameters> _cabac_context_parameters);

    virtual void write(BitWriter *writer) const;

    std::unique_ptr<CabacBinarization> clone() const;

   private:
    CabacBinarizationParameters::BinarizationId binarization_ID;                 //!< : 5; Line 2
    uint8_t bypass_flag : 1;                                                     //!< Line 3
    std::unique_ptr<CabacBinarizationParameters> cabac_binarization_parameters;  //!< Line 4
    std::unique_ptr<CabacContextParameters> cabac_context_parameters;            //!< Line 6
};
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CABAC_BINARIZATION_H
