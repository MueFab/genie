/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_INTERNAL_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_INTERNAL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/core/meta/refbase.h"
#include "genie/format/mgg/reference/location.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace reference {
namespace location {

/**
 * @brief
 */
class Internal : public Location {
 private:
    uint8_t internal_dataset_group_id;  //!< @brief
    uint16_t internal_dataset_id;       //!< @brief

 public:
    /**
     * @brief
     * @param _reserved
     * @param _internal_dataset_group_id
     * @param _internal_dataset_id
     */
    Internal(uint8_t _reserved, uint8_t _internal_dataset_group_id, uint16_t _internal_dataset_id);

    /**
     * @brief
     * @param reader
     */
    explicit Internal(genie::util::BitReader& reader);

    /**
     * @brief
     * @param reader
     * @param _reserved
     */
    Internal(genie::util::BitReader& reader, uint8_t _reserved);

    /**
     * @brief
     * @return
     */
    uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    uint16_t getDatasetID() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) override;

    /**
     * @brief
     * @return
     */
    std::unique_ptr<genie::core::meta::RefBase> decapsulate() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace location
}  // namespace reference
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_INTERNAL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
