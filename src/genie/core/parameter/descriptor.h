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

/**
 * ISO 23092-2 Section 3.3.2.1 table 8
 */
class Descriptor {
   public:
    enum class Preset : uint8_t {
        PRESENT = 0  //!< See text in section 3.3.2.1
    };

    explicit Descriptor(Preset _dec_cfg_preset);

    virtual std::unique_ptr<Descriptor> clone() const = 0;

    virtual void write(util::BitWriter &writer) const;

    static std::unique_ptr<Descriptor> factory(GenDesc desc, util::BitReader &reader);

    virtual ~Descriptor() = default;

    Preset getPreset() const;

   protected:
    Preset dec_cfg_preset : 8;  //!< Line 2
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DESCRIPTOR_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------