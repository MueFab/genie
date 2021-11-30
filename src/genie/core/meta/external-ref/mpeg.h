/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_EXTERNAL_REF_MPEG_H_
#define SRC_GENIE_CORE_META_EXTERNAL_REF_MPEG_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/meta/external-ref.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {
namespace external_ref {

/**
 * @brief
 */
class MPEG : public ExternalRef {
 private:
    uint16_t external_dataset_group_id;  //!< @brief
    uint16_t external_dataset_id;        //!< @brief
    std::string ref_checksum;            //!< @brief

 public:
    /**
     * @brief
     * @param _ref_uri
     * @param check
     * @param group_id
     * @param id
     * @param checksum
     */
    MPEG(std::string _ref_uri, ChecksumAlgorithm check, uint16_t group_id, uint16_t id, std::string checksum);

    /**
     * @brief
     * @param json
     */
    explicit MPEG(const nlohmann::json& json);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const override;

    /**
     * @brief
     * @return
     */
    uint16_t getGroupID() const;

    /**
     * @brief
     * @return
     */
    uint16_t getID() const;

    /**
     * @brief
     * @return
     */
    const std::string& getChecksum() const;
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
