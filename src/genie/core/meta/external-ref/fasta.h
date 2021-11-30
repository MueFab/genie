/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_EXTERNAL_REF_FASTA_H_
#define SRC_GENIE_CORE_META_EXTERNAL_REF_FASTA_H_

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
 * @brief
 */
class Fasta : public ExternalRef {
 private:
    std::vector<std::string> ref_type_other_checksums;  //!< @brief

 public:
    /**
     * @brief
     * @param _ref_uri
     * @param check
     * @param firstChecksum
     */
    Fasta(std::string _ref_uri, ChecksumAlgorithm check, std::string firstChecksum);

    /**
     * @brief
     * @param json
     */
    explicit Fasta(const nlohmann::json& json);

    /**
     * @brief
     * @return
     */
    const std::vector<std::string>& getChecksums() const;

    /**
     * @brief
     * @param checksum
     */
    void addChecksum(std::string checksum);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace external_ref
}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_EXTERNAL_REF_FASTA_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
