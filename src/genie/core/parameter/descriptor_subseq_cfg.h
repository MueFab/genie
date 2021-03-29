/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_SUBSEQ_CFG_H_
#define SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_SUBSEQ_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/parameter/descriptor.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

/**
 *
 */
class DescriptorSubseqCfg {
 public:
    /**
     *
     */
    DescriptorSubseqCfg();

    /**
     *
     * @param cfg
     */
    DescriptorSubseqCfg(const DescriptorSubseqCfg& cfg);

    /**
     *
     * @param cfg
     */
    DescriptorSubseqCfg(DescriptorSubseqCfg&& cfg) noexcept;

    /**
     *
     * @param cfg
     * @return
     */
    DescriptorSubseqCfg& operator=(const DescriptorSubseqCfg& cfg);

    /**
     *
     * @param cfg
     * @return
     */
    DescriptorSubseqCfg& operator=(DescriptorSubseqCfg&& cfg) noexcept;

    /**
     *
     * @param num_classes
     * @param desc
     * @param reader
     */
    DescriptorSubseqCfg(size_t num_classes, GenDesc desc, util::BitReader& reader);

    /**
     *
     */
    virtual ~DescriptorSubseqCfg() = default;

    /**
     *
     * @param index
     * @param conf
     */
    void setClassSpecific(uint8_t index, std::unique_ptr<Descriptor> conf);

    /**
     *
     * @param conf
     */
    void set(std::unique_ptr<Descriptor> conf);

    /**
     *
     * @return
     */
    const Descriptor& get() const;

    /**
     *
     * @param index
     * @return
     */
    const Descriptor& getClassSpecific(uint8_t index) const;

    /**
     *
     * @param numClasses
     */
    void enableClassSpecific(uint8_t numClasses);  //!< Unlocks class specific config

    /**
     *
     * @return
     */
    bool isClassSpecific() const;

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     *
     * @param cfg
     * @return
     */
    bool operator==(const DescriptorSubseqCfg& cfg) const;

 private:
    /**
     *
     * @param cfg
     * @return
     */
    bool desc_comp(const DescriptorSubseqCfg& cfg) const;

    bool class_specific_dec_cfg_flag;                                    //!<
    std::vector<std::unique_ptr<Descriptor>> descriptor_configurations;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_SUBSEQ_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
