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
    explicit ParameterSet(util::BitReader &reader) {
        const size_t num_codebooks = reader.read<int>(4);
        for (auto c = num_codebooks; c > 0; --c) {
            qv_codebooks.emplace_back(Codebook(reader));
        }
    }
    explicit ParameterSet() = default;
    const std::vector<Codebook> &getCodebooks() const { return qv_codebooks; }

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