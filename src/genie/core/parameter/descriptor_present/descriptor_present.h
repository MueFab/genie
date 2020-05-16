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
 *
 */
class DescriptorPresent : public Descriptor {
   private:
    std::unique_ptr<Decoder> decoder_configuration;  //!<

   public:
    static constexpr int8_t PRESENT = 0;  //!<

    /**
     *
     */
    explicit DescriptorPresent();

    /**
     *
     * @param desc
     * @param reader
     */
    explicit DescriptorPresent(GenDesc desc, util::BitReader &reader);

    /**
     *
     * @return
     */
    std::unique_ptr<Descriptor> clone() const override;

    /**
     *
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     *
     * @param conf
     */
    void setDecoder(std::unique_ptr<Decoder> conf);

    /**
     *
     * @return
     */
    const Decoder &getDecoder() const;
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