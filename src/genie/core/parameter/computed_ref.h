/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_H_
#define SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "boost/optional/optional.hpp"
#include "genie/core/parameter/computed_ref_extended.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

/**
 *
 */
class ComputedRef {
 public:
    /**
     *
     */
    enum class Algorithm : uint8_t {
        RESERVED = 0,
        REF_TRANSFORM = 1,
        PUSH_IN = 2,
        LOCAL_ASSEMBLY = 3,
        GLOBAL_ASSEMBLY = 4
    };

 private:
    Algorithm cr_alg_ID;                             //!<
    boost::optional<ComputedRefExtended> extension;  //!<

 public:
    /**
     *
     * @param cr
     * @return
     */
    bool operator==(const ComputedRef &cr) const;

    /**
     *
     * @param _cr_alg_ID
     */
    explicit ComputedRef(Algorithm _cr_alg_ID);

    /**
     *
     * @param reader
     */
    explicit ComputedRef(util::BitReader &reader);

    /**
     *
     */
    virtual ~ComputedRef() = default;

    /**
     *
     * @param _crps_info
     */
    void setExtension(ComputedRefExtended &&_crps_info);

    /**
     *
     * @return
     */
    const ComputedRefExtended &getExtension() const;

    /**
     *
     * @return
     */
    Algorithm getAlgorithm() const;

    /**
     *
     * @param bw
     */
    virtual void write(util::BitWriter &bw) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
