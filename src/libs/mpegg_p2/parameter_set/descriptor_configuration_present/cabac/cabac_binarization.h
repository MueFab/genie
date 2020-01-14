#ifndef GENIE_CABAC_BINARIZATION_H
#define GENIE_CABAC_BINARIZATION_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>

#include "cabac_binarization_parameters.h"
#include "cabac_context_parameters.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_p2 {
namespace desc_conf_pres {
namespace cabac {

/**
 * ISO 23092-2 Section 8.3.3 table
 */
class CabacBinarization {
   public:
    CabacBinarization(const CabacBinarizationParameters::BinarizationId& _binarization_ID,
                      std::unique_ptr<CabacBinarizationParameters> _cabac_binarization_parameters);

    CabacBinarization();

    CabacBinarization(uint8_t coding_subsym_size, uint8_t output_symbol_size, util::BitReader& reader);

    virtual ~CabacBinarization() = default;

    void setContextParameters(std::unique_ptr<CabacContextParameters> _cabac_context_parameters);

    virtual void write(util::BitWriter& writer) const;

    std::unique_ptr<CabacBinarization> clone() const;

    CabacBinarizationParameters::BinarizationId getBinarizationID() const;

    bool getBypassFlag() const;

    const CabacBinarizationParameters* getCabacBinarizationParameters() const;

    const CabacContextParameters* getCabacContextParameters() const;

   private:
    CabacBinarizationParameters::BinarizationId binarization_ID;                 //!< : 5; Line 2
    uint8_t bypass_flag : 1;                                                     //!< Line 3
    std::unique_ptr<CabacBinarizationParameters> cabac_binarization_parameters;  //!< Line 4
    std::unique_ptr<CabacContextParameters> cabac_context_parameters;            //!< Line 6
};
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace genie
// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CABAC_BINARIZATION_H
