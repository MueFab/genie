/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_BINARIZATION_H
#define GENIE_BINARIZATION_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitwriter.h>
#include <boost/optional/optional.hpp>
#include <memory>
#include "binarization_parameters.h"
#include "context.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 * ISO 23092-2 Section 8.3.3 table
 */
class Binarization {
   public:
    Binarization(BinarizationParameters::BinarizationId _binarization_ID,
                 BinarizationParameters&& _cabac_binarization_parameters);

    Binarization();

    Binarization(uint8_t coding_subsym_size, uint8_t output_symbol_size, util::BitReader& reader);

    virtual ~Binarization() = default;

    void setContextParameters(Context&& _cabac_context_parameters);

    virtual void write(util::BitWriter& writer) const;

    BinarizationParameters::BinarizationId getBinarizationID() const;

    bool getBypassFlag() const;

    const BinarizationParameters& getCabacBinarizationParameters() const;

    const Context& getCabacContextParameters() const;

   private:
    BinarizationParameters::BinarizationId binarization_ID;
    uint8_t bypass_flag;
    BinarizationParameters cabac_binarization_parameters;
    boost::optional<Context> cabac_context_parameters;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BINARIZATION_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------