/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_EXTERNAL_REF_MPEG_H_
#define SRC_GENIE_CORE_META_EXTERNAL_REF_MPEG_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include "genie/core/meta/external-ref.h"
#include "genie/util/make_unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {
namespace external_ref {

/**
 * @brief Represents an external ref in MPEG-G format.
 */
class MPEG : public ExternalRef {
 private:
    uint16_t external_dataset_group_id;  //!< @brief Dataset group in external file
    uint16_t external_dataset_id;        //!< @brief Dataset in the dataset group
    std::string ref_checksum;            //!< @brief Checksum of the full reference

 public:
    /**
     * @brief Constructor from raw values
     * @param _ref_uri URI to reference file
     * @param check Algorithm used for checksum
     * @param group_id Dataset group id in external file
     * @param id Dataset id in dataset group
     * @param checksum Checksum of full reference
     */
    MPEG(std::string _ref_uri, ChecksumAlgorithm check, uint16_t group_id, uint16_t id, std::string checksum);

    /**
     * @brief Constructor from json
     * @param json Json representation
     */
    explicit MPEG(const nlohmann::json& json);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    nlohmann::json toJson() const override;

    /**
     * @brief Return dataset group id in external file
     * @return Dataset group id in external file
     */
    uint16_t getGroupID() const;

    /**
     * @brief Return dataset ID in external file
     * @return Dataset ID in external file
     */
    uint16_t getID() const;

    /**
     * @brief Return checksum of full reference
     * @return Checksum of full reference
     */
    const std::string& getChecksum() const;

    /**
     * @brief
     * @return
     */
    std::string& getChecksum();

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

#endif  // SRC_GENIE_CORE_META_EXTERNAL_REF_MPEG_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
