/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_READ_DECODER_H
#define GENIE_READ_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/selector.h>
#include <genie/util/source.h>
#include "access-unit-raw.h"
#include "qv-decoder.h"
#include "record/record.h"
#include "name-decoder.h"
#include "qv-decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief The basic interface for modules decoding the plain read data
 */
class ReadDecoder : public util::Source<record::Chunk>, public util::Drain<AccessUnitRaw> {
   protected:
    genie::util::SideSelector<genie::core::QVDecoder, std::vector<std::string>, const genie::core::parameter::QualityValues&,
        const std::vector<std::string>&, genie::core::AccessUnitRaw::Descriptor&>*
        qvcoder;
    genie::util::SideSelector<genie::core::NameDecoder, std::vector<std::string>,
        genie::core::AccessUnitRaw::Descriptor&>* namecoder;
   public:
    virtual void setQVCoder(genie::util::SideSelector<genie::core::QVDecoder, std::vector<std::string>, const genie::core::parameter::QualityValues&,
        const std::vector<std::string>&, genie::core::AccessUnitRaw::Descriptor&>* coder) {
        qvcoder = coder;
    }

    virtual void setNameCoder(genie::util::SideSelector<genie::core::NameDecoder, std::vector<std::string>,
        genie::core::AccessUnitRaw::Descriptor&>* coder) {
        namecoder = coder;
    }
    /**
     * @Brief For polymorphic destruction
     */
    ~ReadDecoder() override = default;

    void skipIn(const util::Section& sec) override {
        skipOut(sec);
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_READ_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------