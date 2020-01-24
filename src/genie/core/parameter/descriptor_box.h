/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DESCRIPTOR_BOX_H
#define GENIE_DESCRIPTOR_BOX_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <memory>
#include <vector>
#include "descriptor.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

class DescriptorBox {
   public:
    DescriptorBox();

    DescriptorBox(const DescriptorBox& box);

    DescriptorBox(DescriptorBox&& box) noexcept;

    DescriptorBox& operator=(const DescriptorBox& box);

    DescriptorBox& operator=(DescriptorBox&& box) noexcept;

    DescriptorBox(size_t num_classes, GenDesc desc, util::BitReader& reader);

    virtual ~DescriptorBox() = default;

    void setClassSpecific(uint8_t index, std::unique_ptr<Descriptor> conf);  //!< For class specific config
    void set(std::unique_ptr<Descriptor> conf);                              //!< For non-class-specific config
    const Descriptor* get() const;
    const Descriptor* getClassSpecific(uint8_t index) const;
    void enableClassSpecific(uint8_t numClasses);  //!< Unlocks class specific config
    bool isClassSpecific() const;

    virtual void write(util::BitWriter& writer) const;

   private:
    bool class_specific_dec_cfg_flag;
    std::vector<std::unique_ptr<Descriptor>> descriptor_configurations;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DESCRIPTOR_BOX_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------