/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_READ_DECODER_H
#define GENIE_READ_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/record/chunk.h>
#include <genie/util/selector.h>
#include <genie/util/sideSelector.h>
#include <genie/util/source.h>
#include "access-unit.h"
#include "module.h"
#include "name-decoder.h"
#include "qv-decoder.h"
#include "entropy-decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief The basic interface for modules decoding the plain read data
 */
class ReadDecoder : public Module<AccessUnit, record::Chunk> {
   public:
    using QvSelector = util::SideSelector<QVDecoder, std::vector<std::string>, const parameter::QualityValues&,
                                          const std::vector<std::string>&, AccessUnit::Descriptor&>;          //!<
    using NameSelector = util::SideSelector<NameDecoder, std::vector<std::string>, AccessUnit::Descriptor&>;  //!<
    using EntropySelector = util::SideSelector<EntropyDecoder, AccessUnit::Descriptor, const parameter::DescriptorSubseqCfg&, AccessUnit::Descriptor&>;  //!<

   protected:
    QvSelector* qvcoder{};      //!<
    NameSelector* namecoder{};  //!<
    EntropySelector* entropycoder{};  //!<

   public:
    /**
     *
     * @param coder
     */
    virtual void setQVCoder(QvSelector* coder);

    /**
     *
     * @param coder
     */
    virtual void setNameCoder(NameSelector* coder);

    virtual void setEntropyCoder(EntropySelector* coder) {
        entropycoder = coder;
    }

    static AccessUnit entropyCodeAU(EntropySelector* select, AccessUnit&& a) {
        AccessUnit au = std::move(a);
        for(auto &d : au) {
            d = select->process(au.getParameters().getDescriptor(d.getID()), d);
        }
        return au;
    }

    AccessUnit entropyCodeAU(AccessUnit&& a) {
        return entropyCodeAU(entropycoder, std::move(a));
    }

    /**
     * @Brief For polymorphic destruction
     */
    ~ReadDecoder() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_READ_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------