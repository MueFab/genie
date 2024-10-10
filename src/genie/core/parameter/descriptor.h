/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_H_
#define SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/core/constants.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::parameter {

/**
 * @brief
 */
class Descriptor {
 public:
    /**
     * @brief
     * @param _dec_cfg_preset
     */
    explicit Descriptor(uint8_t _dec_cfg_preset);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] virtual std::unique_ptr<Descriptor> clone() const = 0;

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const;

    /**
     * @brief
     * @param desc
     * @param reader
     * @return
     */
    static std::unique_ptr<Descriptor> factory(GenDesc desc, util::BitReader &reader);

    /**
     * @brief
     */
    virtual ~Descriptor() = default;

    /**
     * @brief
     * @param desc
     * @return
     */
    virtual bool equals(const Descriptor *desc) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getPreset() const;

 protected:
    uint8_t dec_cfg_preset;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::parameter

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
