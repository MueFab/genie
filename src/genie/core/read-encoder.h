/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_READ_ENCODER_H
#define GENIE_READ_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/drain.h>
#include <genie/util/selector.h>
#include <genie/util/sideSelector.h>
#include <genie/util/source.h>
#include "access-unit.h"
#include "module.h"
#include "entropy-encoder.h"
#include "name-encoder.h"
#include "qv-encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief The basic interface for modules encoding the plain read data
 */
class ReadEncoder : public Module<record::Chunk, AccessUnit> {
   public:
    using QvSelector = util::SideSelector<QVEncoder, QVEncoder::QVCoded, const record::Chunk&>;             //!<
    using NameSelector = util::SideSelector<NameEncoder, std::tuple<AccessUnit::Descriptor, core::stats::PerfStats>, const record::Chunk&>;  //!<
    using EntropySelector = util::SideSelector<EntropyEncoder, EntropyEncoder::EntropyCoded, AccessUnit::Descriptor&>;  //!<

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

    /**
     *
     * @param coder
     */
    virtual void setEntropyCoder(EntropySelector* coder) {
        entropycoder = coder;
    }

    static AccessUnit entropyCodeAU(EntropySelector* entropycoder, AccessUnit&& a) {
        AccessUnit au = std::move(a);
        for(auto &d : au) {
            auto encoded = entropycoder->process(d);
            au.getParameters().setDescriptor(d.getID(), std::move(std::get<0>(encoded)));
            au.set(d.getID(), std::move(std::get<1>(encoded)));
            au.getStats().add(std::get<2>(encoded));
        }
        return au;
    }

    AccessUnit entropyCodeAU(AccessUnit&& a) {
        return entropyCodeAU(entropycoder, std::move(a));
    }

    /**
     * @Brief For polymorphic destruction
     */
    ~ReadEncoder() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_READ_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------