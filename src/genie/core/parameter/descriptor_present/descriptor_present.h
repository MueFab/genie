/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DESCRIPTOR_PRESENT_H_
#define SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DESCRIPTOR_PRESENT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/core/parameter/descriptor.h"
#include "genie/core/parameter/descriptor_present/decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::parameter::desc_pres {

/**
 * @brief
 */
class DescriptorPresent : public Descriptor {
 private:
    std::unique_ptr<Decoder> decoder_configuration;  //!< @brief

 public:
    static constexpr int8_t PRESENT = 0;  //!< @brief

    /**
     * @brief
     * @param desc
     * @return
     */
    bool equals(const Descriptor *desc) const override;

    /**
     * @brief
     */
    DescriptorPresent();

    /**
     * @brief
     * @param desc
     * @param reader
     */
    explicit DescriptorPresent(GenDesc desc, util::BitReader &reader);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] std::unique_ptr<Descriptor> clone() const override;

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     * @brief
     * @param conf
     */
    void setDecoder(std::unique_ptr<Decoder> conf);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const Decoder &getDecoder() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::parameter::desc_pres

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DESCRIPTOR_PRESENT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
