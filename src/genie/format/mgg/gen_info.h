/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_GEN_INFO_H_
#define SRC_GENIE_FORMAT_MGG_GEN_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/format/mgg/box.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief
 */
class GenInfo : public Box {
 protected:
    /**
     * @brief
     * @param bitWriter
     */
    virtual void box_write(genie::util::BitWriter& bitWriter) const = 0;

 public:
    /**
     * @brief
     * @return
     */
    static uint64_t getHeaderLength();

    /**
     * @brief
     * @return
     */
    [[nodiscard]] virtual const std::string& getKey() const = 0;

    /**
     * @brief
     * @param bitWriter
     */
    void write(genie::util::BitWriter& bitWriter) const final;

    /**
     * @brief
     * @param info
     * @return
     */
    virtual bool operator==(const GenInfo& info) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_GEN_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
