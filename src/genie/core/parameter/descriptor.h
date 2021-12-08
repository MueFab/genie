/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_H_
#define SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

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
    virtual std::unique_ptr<Descriptor> clone() const = 0;

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
    uint8_t getPreset() const;

 protected:
    uint8_t dec_cfg_preset;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
