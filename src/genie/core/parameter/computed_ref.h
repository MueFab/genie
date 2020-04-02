/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_COMPUTED_REF_H
#define GENIE_COMPUTED_REF_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <boost/optional/optional.hpp>
#include <cstdint>
#include <memory>
#include "computed_ref_extended.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

class ComputedRef {
   public:
    enum class Algorithm : uint8_t {
        RESERVED = 0,
        REF_TRANSFORM = 1,
        PUSH_IN = 2,
        LOCAL_ASSEMBLY = 3,
        GLOBAL_ASSEMBLY = 4
    };

   private:
    Algorithm cr_alg_ID;
    boost::optional<ComputedRefExtended> extension;  // TODO: std::optional

   public:
    explicit ComputedRef(Algorithm _cr_alg_ID);

    explicit ComputedRef(util::BitReader &reader);

    virtual ~ComputedRef() = default;

    void setExtension(ComputedRefExtended &&_crps_info);

    const ComputedRefExtended &getExtension() const;

    Algorithm getAlgorithm() const{
        return cr_alg_ID;
    }

    virtual void write(util::BitWriter &bw) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_COMPUTED_REF_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------