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

namespace genie::core::parameter {

/**
 * @brief
 */
class ComputedRefExtended {
 private:
    uint8_t cr_pad_size;       //!< @brief
    uint32_t cr_buf_max_size;  //!< @brief

 public:
    /**
     * @brief
     * @param ext
     * @return
     */
    bool operator==(const ComputedRefExtended& ext) const;

    /**
     * @brief
     * @param cr_pad_size
     * @param cr_buf_max_size
     */
    ComputedRefExtended(uint8_t cr_pad_size, uint32_t cr_buf_max_size);

    /**
     * @brief
     */
    virtual ~ComputedRefExtended() = default;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint32_t getBufMaxSize() const;

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::parameter

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_EXTENDED_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
