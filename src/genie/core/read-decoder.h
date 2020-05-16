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
#include "module.h"
#include "name-decoder.h"
#include "qv-decoder.h"
#include "record/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief The basic interface for modules decoding the plain read data
 */
class ReadDecoder : public Module<AccessUnitRaw, record::Chunk> {
   public:
    using QvSelector = util::SideSelector<QVDecoder, std::vector<std::string>, const parameter::QualityValues&,
                                          const std::vector<std::string>&, AccessUnitRaw::Descriptor&>;          //!<
    using NameSelector = util::SideSelector<NameDecoder, std::vector<std::string>, AccessUnitRaw::Descriptor&>;  //!<

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
    ~ReadDecoder() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_READ_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------