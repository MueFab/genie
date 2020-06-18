/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_COMPUTED_REF_EXTENDED_H
#define GENIE_COMPUTED_REF_EXTENDED_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <cstdint>
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

/**
 *
 */
class ComputedRefExtended {
   private:
    uint8_t cr_pad_size;       //!<
    uint32_t cr_buf_max_size;  //!<

   public:
    /**
     *
     * @param cr_pad_size
     * @param cr_buf_max_size
     */
    ComputedRefExtended(uint8_t cr_pad_size, uint32_t cr_buf_max_size);

    /**
     *
     */
    virtual ~ComputedRefExtended() = default;

    /**
     *
     * @return
     */
    uint32_t getBufMaxSize() const;

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_COMPUTED_REF_EXTENDED_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------