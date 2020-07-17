/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_RAW_REFERENCE_H
#define GENIE_RAW_REFERENCE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/parameter/data_unit.h>
#include <genie/util/bitwriter.h>
#include <cstdint>
#include <vector>
#include "raw_reference_seq.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 *
 */
class RawReference : public core::parameter::DataUnit {
   private:
    std::vector<RawReferenceSequence> seqs;  //!<

   public:
    /**
     *
     */
    RawReference();

    /**
     *
     * @param reader
     * @param headerOnly
     */
    explicit RawReference(util::BitReader& reader, bool headerOnly = false);

    /**
     *
     */
    ~RawReference() override = default;

    /**
     *
     * @param ref
     */
    void addSequence(RawReferenceSequence&& ref);

    /**
     *
     * @param writer
     */
    void write(util::BitWriter& writer) const override;

    /**
     *
     * @return
     */
    bool isEmpty() const;

    /**
     *
     * @param index
     * @return
     */
    RawReferenceSequence& getSequence(size_t index);

    /**
     *
     * @return
     */
    std::vector<RawReferenceSequence>::iterator begin();

    /**
     *
     * @return
     */
    std::vector<RawReferenceSequence>::iterator end();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_RAW_REFERENCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
