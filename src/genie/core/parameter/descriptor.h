/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DESCRIPTOR_H
#define GENIE_DESCRIPTOR_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/constants.h>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {


class Descriptor {
   public:

    explicit Descriptor(uint8_t _dec_cfg_preset);

    virtual std::unique_ptr<Descriptor> clone() const = 0;

    virtual void write(util::BitWriter &writer) const;

    static std::unique_ptr<Descriptor> factory(GenDesc desc, util::BitReader &reader);

    virtual ~Descriptor() = default;

    uint8_t getPreset() const;

   protected:
    uint8_t dec_cfg_preset;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DESCRIPTOR_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------