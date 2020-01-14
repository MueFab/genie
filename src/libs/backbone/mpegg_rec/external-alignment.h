/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_EXTERNAL_ALIGNMENT_H
#define GENIE_EXTERNAL_ALIGNMENT_H

// ---------------------------------------------------------------------------------------------------------------------

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <cstdint>
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_rec {

/**
 *
 */
class ExternalAlignment {
   public:
    /**
     *
     */
    enum class MoreAlignmentInfoType : uint8_t { NONE, OTHER_REC };

    /**
     *
     * @param _moreAlignmentInfoType
     */
    explicit ExternalAlignment(MoreAlignmentInfoType _moreAlignmentInfoType);

    /**
     *
     */
    virtual ~ExternalAlignment() = default;

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const;

    /**
     *
     * @param reader
     * @return
     */
    static std::unique_ptr<ExternalAlignment> factory(util::BitReader &reader);

    /**
     *
     * @return
     */
    virtual std::unique_ptr<ExternalAlignment> clone() const = 0;

   protected:
    MoreAlignmentInfoType moreAlignmentInfoType;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_EXTERNAL_ALIGNMENT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------