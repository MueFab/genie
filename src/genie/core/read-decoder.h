/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_READ_DECODER_H_
#define SRC_GENIE_CORE_READ_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <tuple>
#include <vector>
#include "genie/core/access-unit.h"
#include "genie/core/entropy-decoder.h"
#include "genie/core/module.h"
#include "genie/core/name-decoder.h"
#include "genie/core/qv-decoder.h"
#include "genie/core/record/alignment/chunk.h"
#include "genie/util/selector.h"
#include "genie/util/side-selector.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief The basic interface for modules decoding the plain read data
 */
class ReadDecoder : public Module<AccessUnit, record::Chunk> {
 public:
    using QvSelector = util::SideSelector<QVDecoder, std::tuple<std::vector<std::string>, core::stats::PerfStats>,
                                          const parameter::QualityValues&, const std::vector<std::string>&,
                                          const std::vector<uint64_t>&, AccessUnit::Descriptor&>;  //!< @brief
    using NameSelector = util::SideSelector<NameDecoder, std::tuple<std::vector<std::string>, core::stats::PerfStats>,
                                            AccessUnit::Descriptor&>;  //!< @brief
    using EntropySelector =
        util::SideSelector<EntropyDecoder, std::tuple<AccessUnit::Descriptor, core::stats::PerfStats>,
                           const parameter::DescriptorSubseqCfg&, AccessUnit::Descriptor&, bool>;  //!< @brief

 protected:
    QvSelector* qvcoder{};            //!< @brief
    NameSelector* namecoder{};        //!< @brief
    EntropySelector* entropycoder{};  //!< @brief

 public:
    /**
     * @brief
     * @param coder
     */
    virtual void setQVCoder(QvSelector* coder);

    /**
     * @brief
     * @param coder
     */
    virtual void setNameCoder(NameSelector* coder);

    /**
     * @brief
     * @param coder
     */
    virtual void setEntropyCoder(EntropySelector* coder);

    /**
     * @brief
     * @param select
     * @param a
     * @param mmCoderEnabled
     * @return
     */
    static AccessUnit entropyCodeAU(EntropySelector* select, AccessUnit&& a, bool mmCoderEnabled);

    /**
     * @brief
     * @param a
     * @param mmCoderEnabled
     * @return
     */
    AccessUnit entropyCodeAU(AccessUnit&& a, bool mmCoderEnabled);

    /**
     * @brief For polymorphic destruction
     */
    ~ReadDecoder() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_READ_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
