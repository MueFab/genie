/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_EXTENDED_H_
#define SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_EXTENDED_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "genie/util/bitwriter.h"

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
     * @param ext
     * @return
     */
    bool operator==(const ComputedRefExtended& ext) const;

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
    virtual void write(util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_EXTENDED_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
