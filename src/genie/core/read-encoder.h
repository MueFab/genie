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
#include <genie/util/side-selector.h>
#include <genie/util/source.h>
#include "access-unit.h"
#include "entropy-encoder.h"
#include "module.h"
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
    using QvSelector = util::SideSelector<QVEncoder, QVEncoder::QVCoded, const record::Chunk&>;  //!<
    using NameSelector = util::SideSelector<NameEncoder, std::tuple<AccessUnit::Descriptor, core::stats::PerfStats>,
                                            const record::Chunk&>;  //!<
    using EntropySelector =
        util::SideSelector<EntropyEncoder, EntropyEncoder::EntropyCoded, AccessUnit::Descriptor&>;  //!<

   protected:
    QvSelector* qvcoder{};            //!<
    NameSelector* namecoder{};        //!<
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
    virtual void setEntropyCoder(EntropySelector* coder);

    /**
     *
     * @param entropycoder
     * @param a
     * @return
     */
    static AccessUnit entropyCodeAU(EntropySelector* entropycoder, AccessUnit&& a);

    /**
     *
     * @param a
     * @return
     */
    AccessUnit entropyCodeAU(AccessUnit&& a);

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