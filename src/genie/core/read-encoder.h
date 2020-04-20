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
#include <genie/util/source.h>
#include "access-unit-raw.h"
#include "name-encoder.h"
#include "qv-encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief The basic interface for modules encoding the plain read data
 */
class ReadEncoder : public util::Drain<record::Chunk>, public util::Source<AccessUnitRaw> {
   protected:
    util::SideSelector<genie::core::QVEncoder, genie::core::QVEncoder::QVCoded, const genie::core::record::Chunk&>*
        qvcoder;
    util::SideSelector<genie::core::NameEncoder, genie::core::AccessUnitRaw::Descriptor,
                       const genie::core::record::Chunk&>* namecoder;

   public:
    virtual void setQVCoder(util::SideSelector<genie::core::QVEncoder, genie::core::QVEncoder::QVCoded,
                                               const genie::core::record::Chunk&>* coder) {
        qvcoder = coder;
    }

    virtual void setNameCoder(util::SideSelector<genie::core::NameEncoder, genie::core::AccessUnitRaw::Descriptor,
                                                 const genie::core::record::Chunk&>* coder) {
        namecoder = coder;
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