/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DESC_DECODER_H
#define GENIE_DESC_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>

#include <genie/util/bitwriter.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {
namespace desc_pres {

/**
 *
 */
class Decoder {
   public:
    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     *
     * @return
     */
    virtual std::unique_ptr<Decoder> clone() const = 0;

    /**
     *
     * @param _encoding_mode_id
     */
    explicit Decoder(uint8_t _encoding_mode_id);

    /**
     *
     */
    virtual ~Decoder() = default;

    /**
     *
     * @return
     */
    uint8_t getMode() const;

    /**
     *
     * @param dec
     * @return
     */
    virtual bool equals(const Decoder* dec) const;

   protected:
    uint8_t encoding_mode_ID;  //!<
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