/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_H_
#define SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <optional>
#include "genie/core/parameter/computed_ref_extended.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::parameter {

/**
 * @brief
 */
class ComputedRef {
 public:
    /**
     * @brief
     */
    enum class Algorithm : uint8_t {
        RESERVED = 0,
        REF_TRANSFORM = 1,
        PUSH_IN = 2,
        LOCAL_ASSEMBLY = 3,
        GLOBAL_ASSEMBLY = 4
    };

 private:
    Algorithm cr_alg_ID;                           //!< @brief
    std::optional<ComputedRefExtended> extension;  //!< @brief

 public:
    /**
     * @brief
     * @param cr
     * @return
     */
    bool operator==(const ComputedRef &cr) const;

    /**
     * @brief
     * @param _cr_alg_ID
     */
    explicit ComputedRef(Algorithm _cr_alg_ID);

    /**
     * @brief
     * @param reader
     */
    explicit ComputedRef(util::BitReader &reader);

    /**
     * @brief
     */
    virtual ~ComputedRef() = default;

    /**
     * @brief
     * @param _crps_info
     */
    void setExtension(ComputedRefExtended &&_crps_info);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const ComputedRefExtended &getExtension() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] Algorithm getAlgorithm() const;

    /**
     * @brief
     * @param bw
     */
    virtual void write(util::BitWriter &bw) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::parameter

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
