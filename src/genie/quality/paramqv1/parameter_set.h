/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_PARAMETER_SET_H
#define GENIE_PARAMETER_SET_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/make-unique.h>
#include <cstdint>
#include <memory>
#include <vector>

#include <genie/util/bitwriter.h>
#include "codebook.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace paramqv1 {

class ParameterSet {
   private:
    std::vector<Codebook> qv_codebooks;

   public:
    explicit ParameterSet(util::BitReader &reader);
    explicit ParameterSet() = default;
    const std::vector<Codebook> &getCodebooks() const;

    virtual ~ParameterSet() = default;

    void addCodeBook(Codebook &&book);

    virtual void write(util::BitWriter &writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_PARAMETER_SET_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------