/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_EXTERNAL_REF_RAW_H_
#define SRC_GENIE_CORE_META_EXTERNAL_REF_RAW_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/core/meta/external-ref.h"

#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::meta::external_ref {

/**
 * @brief Represents external raw reference
 */
class Raw : public ExternalRef {
 private:
    std::vector<std::string> ref_type_other_checksums;  //!< @brief Checksums of sequences

 public:
    /**
     * @brief Construct from raw data
     * @param _ref_uri URI to reference file
     * @param check Algorithm used to compute checksums
     */
    Raw(std::string _ref_uri, ChecksumAlgorithm check);

    /**
     * @brief Construct from json
     * @param json Json representation
     */
    explicit Raw(const nlohmann::json& json);

    /**
     * @brief Return sequence checksums
     * @return Sequence checksums
     */
    [[nodiscard]] const std::vector<std::string>& getChecksums() const;

    /**
     * @brief
     * @return
     */
    std::vector<std::string>& getChecksums();

    /**
     * @brief Add new checksum
     * @param checksum Checksum to add
     */
    void addChecksum(std::string checksum);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    [[nodiscard]] nlohmann::json toJson() const override;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] std::unique_ptr<RefBase> clone() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::meta::external_ref

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_EXTERNAL_REF_RAW_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
