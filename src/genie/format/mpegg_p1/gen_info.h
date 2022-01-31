/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GEN_INFO_H
#define GENIE_GEN_INFO_H

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class GenInfo {
 public:
    /**
     * @brief
     * @return
     */
    virtual const std::string& getKey() const = 0;

    /**
     * @brief
     * @return
     */
    virtual uint64_t getSize() const;

    /**
     * @brief
     * @param bitWriter
     */
    virtual void write(genie::util::BitWriter& bitWriter) const;

    /**
     * @brief
     */
    virtual ~GenInfo() = default;

    /**
     * @brief
     * @param info
     * @return
     */
    virtual bool operator==(const GenInfo& info) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GEN_INFO_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
