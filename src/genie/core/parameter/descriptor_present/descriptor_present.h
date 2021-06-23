/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DESCRIPTOR_PRESENT_H_
#define SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DESCRIPTOR_PRESENT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/core/parameter/descriptor.h"
#include "genie/core/parameter/descriptor_present/decoder.h"

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
     * @param desc
     * @return
     */
    bool equals(const Descriptor *desc) const override;

    /**
     *
     */
    DescriptorPresent();

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

#endif  // SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DESCRIPTOR_PRESENT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
