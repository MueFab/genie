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
#include <memory>
#include "genie/core/meta/external-ref.h"
#include "genie/util/make-unique.h"
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {
namespace external_ref {

/**
 * @brief Represents a reference in fasta format.
 */
class Fasta : public ExternalRef {
 private:
    std::vector<std::string> ref_type_other_checksums;  //!< @brief Sequence checksums

 public:
    /**
     * @brief Construct form raw values
     * @param _ref_uri URI to reference
     * @param check Checksum algorithm used
     */
    Fasta(std::string _ref_uri, ChecksumAlgorithm check);

    /**
     * @brief Construct from json
     * @param json Json input
     */
    explicit Fasta(const nlohmann::json& json);

    /**
     * @brief Get Checksums of sequences
     * @return Sequence checksums
     */
    const std::vector<std::string>& getChecksums() const;

    /**
     * @brief
     * @return
     */
    std::vector<std::string>& getChecksums();

    /**
     * @brief Add new checksum to the end of the list
     * @param checksum New checksum
     */
    void addChecksum(std::string checksum);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    nlohmann::json toJson() const override;

    /**
     * @brief
     * @return
     */
    std::unique_ptr<RefBase> clone() const override;
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
