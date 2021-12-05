/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_EXTERNAL_REF_H_
#define SRC_GENIE_CORE_META_EXTERNAL_REF_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/meta/refbase.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

/**
 * @brief
 */
class ExternalRef : public RefBase {
 public:
    /**
     * @brief
     */
    enum class ChecksumAlgorithm : uint8_t { MD5 = 0, SHA256 = 1 };

 private:
    std::string ref_uri;             //!< @brief
    ChecksumAlgorithm checksum_alg;  //!< @brief
    ReferenceType reference_type;    //!< @brief

 public:
    /**
     * @brief
     * @param _ref_uri
     * @param check
     * @param ref
     */
    ExternalRef(std::string _ref_uri, ChecksumAlgorithm check, ReferenceType ref);

    /**
     * @brief
     * @param json
     */
    explicit ExternalRef(const nlohmann::json& json);

    /**
     * @brief
     * @return
     */
    const std::string& getURI() const;

    /**
     * @brief
     * @return
     */
    ChecksumAlgorithm getChecksumAlgo() const;

    /**
     * @brief
     * @return
     */
    ReferenceType getReferenceType() const;

    /**
     * @brief
     * @return
     */
    const std::string& getKeyName() const override;

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_EXTERNAL_REF_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
