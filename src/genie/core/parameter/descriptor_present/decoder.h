/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_H_
#define SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::parameter::desc_pres {

/**
 * @brief
 */
class Decoder {
 public:
    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] virtual std::unique_ptr<Decoder> clone() const = 0;

    /**
     * @brief
     * @param _encoding_mode_id
     */
    explicit Decoder(uint8_t _encoding_mode_id);

    /**
     * @brief
     */
    virtual ~Decoder() = default;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getMode() const;

    /**
     * @brief
     * @param dec
     * @return
     */
    virtual bool equals(const Decoder* dec) const;

 protected:
    uint8_t encoding_mode_ID;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::parameter::desc_pres

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
