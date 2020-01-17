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

/**
 * ISO 23092-2 Section 3.3.2.2 table 9
 */
class ParameterSetQvps {
   private:
    // uint8_t qv_num_codebooks_total : 4;    //!< Line 2
    std::vector<QvCodebook> qv_codebooks;  //!< Lines 3 to 8

   public:
    ParameterSetQvps();

    virtual ~ParameterSetQvps() = default;

    void addCodeBook(const QvCodebook &book);

    virtual void write(util::BitWriter &writer) const;

    std::unique_ptr<ParameterSetQvps> clone() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_PARAMETER_SET_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------