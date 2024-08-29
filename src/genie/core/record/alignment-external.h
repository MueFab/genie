/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief
 */
class AlignmentExternal {
 public:
    /**
     * @brief
     */
    enum class Type : uint8_t { NONE, OTHER_REC };

    /**
     * @brief
     * @param _moreAlignmentInfoType
     */
    explicit AlignmentExternal(Type _moreAlignmentInfoType);

    /**
     * @brief
     */
    virtual ~AlignmentExternal() = default;

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const;

    /**
     * @brief
     * @param reader
     * @return
     */
    static std::unique_ptr<AlignmentExternal> factory(util::BitReader &reader);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] virtual std::unique_ptr<AlignmentExternal> clone() const = 0;

 protected:
    Type moreAlignmentInfoType;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
