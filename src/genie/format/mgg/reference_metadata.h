/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_METADATA_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_METADATA_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief
 */
class ReferenceMetadata : public GenInfo {
 private:
    uint8_t dataset_group_id;              //!< @brief
    uint8_t reference_id;                  //!< @brief
    std::string reference_metadata_value;  //!< @brief

 public:
    /**
     * @brief
     * @param groupID
     */
    void patchID(uint8_t groupID);

    /**
     * @brief
     * @param _old
     * @param _new
     */
    void patchRefID(uint8_t _old, uint8_t _new);

    /**
     * @brief
     * @return
     */
    std::string decapsulate();

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief
     * @param _dataset_group_id
     * @param _reference_id
     * @param _reference_metadata_value
     */
    ReferenceMetadata(uint8_t _dataset_group_id, uint8_t _reference_id, std::string _reference_metadata_value);

    /**
     * @brief
     * @param bitreader
     */
    explicit ReferenceMetadata(genie::util::BitReader& bitreader);

    /**
     * @brief
     * @param bitWriter
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getReferenceID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::string& getReferenceMetadataValue() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_METADATA_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
