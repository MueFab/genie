/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_TRANSFORMED_SEQ_H
#define GENIE_TRANSFORMED_SEQ_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <memory>
#include "binarization.h"
#include "support_values.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 * ISO 23092-2 Section 8.3.1 table lines 9 to 11
 */
class TransformedSeq {
   public:
    TransformedSeq();

    TransformedSeq(SupportValues::TransformIdSubsym _transform_ID_subsym,
                   std::unique_ptr<SupportValues> _support_values, std::unique_ptr<Binarization> _cabac_binarization);

    TransformedSeq(util::BitReader& reader);

    virtual ~TransformedSeq() = default;

    virtual void write(util::BitWriter& writer) const;

    std::unique_ptr<TransformedSeq> clone() const;

    SupportValues::TransformIdSubsym getTransformID() const;

    const SupportValues* getSupportValues() const;

    const Binarization* getBinarization() const;

   private:
    SupportValues::TransformIdSubsym transform_ID_subsym;  //!< : 3; Line 9
    std::unique_ptr<SupportValues> support_values;         //!< Line 10
    std::unique_ptr<Binarization> cabac_binarization;      //!< Line 11
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_TRANSFORMED_SEQ_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------