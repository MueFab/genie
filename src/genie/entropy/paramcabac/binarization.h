/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/entropy/paramcabac/binarization_parameters.h"
#include "genie/entropy/paramcabac/context.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 * @brief
 */
class Binarization {
 public:
    /**
     * @brief
     */
    Binarization();

    /**
     * @brief
     * @param _binarization_ID
     * @param _bypass_flag
     * @param _cabac_binarization_parameters
     * @param _cabac_context_parameters
     */
    Binarization(BinarizationParameters::BinarizationId _binarization_ID, bool _bypass_flag,
                 BinarizationParameters&& _cabac_binarization_parameters, Context&& _cabac_context_parameters);

    /**
     * @brief
     * @param output_symbol_size
     * @param coding_subsym_size
     * @param reader
     */
    Binarization(uint8_t output_symbol_size, uint8_t coding_subsym_size, util::BitReader& reader);

    /**
     * @brief
     */
    virtual ~Binarization() = default;

    /**
     * @brief
     * @param _cabac_context_parameters
     */
    void setContextParameters(Context&& _cabac_context_parameters);

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    BinarizationParameters::BinarizationId getBinarizationID() const;

    /**
     * @brief
     * @return
     */
    bool getBypassFlag() const;

    /**
     * @brief
     * @return
     */
    const BinarizationParameters& getCabacBinarizationParameters() const;

    /**
     * @brief
     * @return
     */
    const Context& getCabacContextParameters() const;

    /**
     * @brief
     * @return
     */
    uint8_t getNumBinarizationParams();

    /**
     * @brief
     * @param bin
     * @return
     */
    bool operator==(const Binarization& bin) const;

    /**
     * @brief
     * @param j
     */
    explicit Binarization(nlohmann::json j);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const;

 private:
    BinarizationParameters::BinarizationId binarization_ID;  //!< @brief
    uint8_t bypass_flag;                                     //!< @brief
    BinarizationParameters cabac_binarization_parameters;    //!< @brief
    Context cabac_context_parameters;                        //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
