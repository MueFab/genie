/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_EXTERNAL_REF_RAW_H_
#define SRC_GENIE_CORE_META_EXTERNAL_REF_RAW_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/core/meta/external-ref.h"
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {
namespace external_ref {

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
    const std::vector<std::string>& getChecksums() const;

    /**
     * @brief Add new checksum
     * @param checksum Checksum to add
     */
    void addChecksum(std::string checksum);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    nlohmann::json toJson() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace external_ref
}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_EXTERNAL_REF_RAW_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
