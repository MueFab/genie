/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_PROTECTION_H_
#define SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_PROTECTION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief
 */
class DescriptorStreamProtection : public GenInfo {
 private:
    std::string DSProtectionValue;  //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    std::string decapsulate();

    /**
     * @brief
     * @param _DSProtectionValue
     */
    explicit DescriptorStreamProtection(std::string _DSProtectionValue);

    /**
     * @brief
     * @param reader
     */
    explicit DescriptorStreamProtection(genie::util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::string& getProtectionValue() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief
     * @param bitWriter
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_PROTECTION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
