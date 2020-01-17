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
 * ISO 23092-2 Section 3.3.2.3 table 16 lines 3+4
 */
class ComputedRefExtended {
   private:
    uint8_t cr_pad_size : 8;        //!<< Line 3
    uint32_t cr_buf_max_size : 24;  //!<< Line 4
   public:
    ComputedRefExtended(uint8_t cr_pad_size, uint32_t cr_buf_max_size);

    ComputedRefExtended();

    virtual ~ComputedRefExtended() = default;

    uint32_t getBufMaxSize() const;

    virtual void write(util::BitWriter &writer) const;

    std::unique_ptr<ComputedRefExtended> clone() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_COMPUTED_REF_EXTENDED_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------