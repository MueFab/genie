/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

/**
 * @brief
 */
class AlignmentSplit {
 public:
    /**
     * @brief
     */
    enum class Type : uint8_t { SAME_REC = 0, OTHER_REC = 1, UNPAIRED = 2 };

    /**
     * @brief
     * @param _split_alignment
     */
    explicit AlignmentSplit(Type _split_alignment);

    /**
     * @brief
     */
    virtual ~AlignmentSplit() = default;

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const;

    /**
     * @brief
     * @param as_depth
     * @param reader
     * @return
     */
    static std::unique_ptr<AlignmentSplit> factory(uint8_t as_depth, util::BitReader &reader);

    /**
     * @brief
     * @return
     */
    virtual std::unique_ptr<AlignmentSplit> clone() const = 0;

    /**
     * @brief
     * @return
     */
    Type getType() const;

 protected:
    Type split_alignment;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
