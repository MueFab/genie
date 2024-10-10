/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_EXTERNAL_REF_H_
#define SRC_GENIE_CORE_META_EXTERNAL_REF_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include "genie/core/meta/refbase.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Information needed to use an external reference - interface
 */
class ExternalRef : public RefBase {
 public:
    /**
     * @brief Checksum algorithm used for checksums
     */
    enum class ChecksumAlgorithm : uint8_t { MD5 = 0, SHA256 = 1 };

 private:
    std::string ref_uri;             //!< @brief URI to reference location
    ChecksumAlgorithm checksum_alg;  //!< @brief Checksum algorithm used
    ReferenceType reference_type;    //!< @brief Type of external reference

 public:
    /**
     * @brief Construct from raw values
     * @param _ref_uri URI to reference location
     * @param check Checksum algorithm used
     * @param ref Type of external reference
     */
    ExternalRef(std::string _ref_uri, ChecksumAlgorithm check, ReferenceType ref);

    /**
     * @brief Construct from json
     * @param json Json representation
     */
    explicit ExternalRef(const nlohmann::json& json);

    /**
     * @brief Return URI to external reference
     * @return URI
     */
    [[nodiscard]] const std::string& getURI() const;

    /**
     * @brief
     * @return
     */
    std::string& getURI();

    /**
     * @brief Return active checksum algorithm
     * @return Checksum algorithm used
     */
    [[nodiscard]] ChecksumAlgorithm getChecksumAlgo() const;

    /**
     * @brief Return reference type
     * @return Reference type
     */
    [[nodiscard]] ReferenceType getReferenceType() const;

    /**
     * @brief Returns "external_ref"
     * @return "external_ref"
     */
    [[nodiscard]] const std::string& getKeyName() const override;

    /**
     * @brief Convert to json
     * @return Json representation
     */
    [[nodiscard]] nlohmann::json toJson() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::meta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_EXTERNAL_REF_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
