/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DESC_DECODER_H
#define GENIE_DESC_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/constants.h>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {
namespace desc_pres {

class Decoder {
   public:
    virtual void write(util::BitWriter& writer) const;

    virtual std::unique_ptr<Decoder> clone() const = 0;

    explicit Decoder(uint8_t _encoding_mode_id);

    virtual ~Decoder() = default;

    uint8_t getMode() const;

   protected:
    uint8_t encoding_mode_ID;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace desc_pres
}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------