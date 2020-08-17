/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_BINARIZATION_H
#define GENIE_BINARIZATION_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitwriter.h>
#include "binarization_parameters.h"
#include "context.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 *
 */
class Binarization {
   public:
    /**
     *
     */
    Binarization();

    /**
     *
     * @param _binarization_ID
     * @param _bypass_flag
     * @param _cabac_binarization_parameters
     * @param _cabac_context_parameters
     */
    Binarization(BinarizationParameters::BinarizationId _binarization_ID, bool _bypass_flag,
                 BinarizationParameters&& _cabac_binarization_parameters, Context&& _cabac_context_parameters);

    /**
     *
     * @param output_symbol_size
     * @param coding_subsym_size
     * @param reader
     */
    Binarization(uint8_t output_symbol_size, uint8_t coding_subsym_size, util::BitReader& reader);

    /**
     *
     */
    virtual ~Binarization() = default;

    /**
     *
     * @param _cabac_context_parameters
     */
    void setContextParameters(Context&& _cabac_context_parameters);

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     *
     * @return
     */
    BinarizationParameters::BinarizationId getBinarizationID() const;

    /**
     *
     * @return
     */
    bool getBypassFlag() const;

    /**
     *
     * @return
     */
    const BinarizationParameters& getCabacBinarizationParameters() const;

    /**
     *
     * @return
     */
    const Context& getCabacContextParameters() const;

    /**
     *
     * @return
     */
    uint8_t getNumBinarizationParams();

    bool operator==(const Binarization& bin) const {
        return binarization_ID == bin.binarization_ID && bypass_flag == bin.bypass_flag &&
               cabac_binarization_parameters == bin.cabac_binarization_parameters &&
               cabac_context_parameters == bin.cabac_context_parameters;
    }

   private:
    BinarizationParameters::BinarizationId binarization_ID;  //!<
    uint8_t bypass_flag;                                     //!<
    BinarizationParameters cabac_binarization_parameters;    //!<
    Context cabac_context_parameters;                        //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BINARIZATION_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
