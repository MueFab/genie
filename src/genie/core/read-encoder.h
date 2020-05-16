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
#include "access-unit-raw.h"
#include "module.h"
#include "name-encoder.h"
#include "qv-encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief The basic interface for modules encoding the plain read data
 */
class ReadEncoder : public Module<record::Chunk, AccessUnitRaw> {
   public:
    using QvSelector = util::SideSelector<QVEncoder, QVEncoder::QVCoded, const record::Chunk&>;             //!<
    using NameSelector = util::SideSelector<NameEncoder, AccessUnitRaw::Descriptor, const record::Chunk&>;  //!<

   protected:
    QvSelector* qvcoder{};      //!<
    NameSelector* namecoder{};  //!<

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