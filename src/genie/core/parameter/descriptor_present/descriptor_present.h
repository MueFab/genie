/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DESCRIPTOR_PRESENT_H
#define GENIE_DESCRIPTOR_PRESENT_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <memory>
#include "decoder.h"
#include "parameter/descriptor.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {
namespace desc_pres {

/**
 * ISO 23092-2 Section 3.3.2.1 table 8, lines 4 to 9
 */
class DescriptorPresent : public Descriptor {
   private:
    std::unique_ptr<Decoder> decoder_configuration;  //!< Line 5 to 9 (fused)
   public:
    explicit DescriptorPresent();

    explicit DescriptorPresent(GenDesc desc, util::BitReader &reader);

    std::unique_ptr<Descriptor> clone() const override;

    void write(util::BitWriter &writer) const override;

    void setDecoder(std::unique_ptr<Decoder> conf);

    const Decoder *getDecoder() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace desc_pres
}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DESCRIPTOR_PRESENT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------