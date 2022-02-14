/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_DECAPSULATED_DATASET_GROUP_H_
#define SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_DECAPSULATED_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <utility>
#include "genie/format/mgb/mgb_file.h"
#include "genie/format/mgg/dataset_group.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace encapsulator {

/**
 * @brief
 */
class DecapsulatedDatasetGroup {
 private:
    uint64_t id{};                                                                                //!< @brief
    boost::optional<genie::core::meta::DatasetGroup> meta_group;                                  //!< @brief
    std::map<uint8_t, genie::core::meta::Reference> meta_references;                              //!< @brief
    std::map<uint64_t, std::pair<genie::format::mgb::MgbFile, genie::core::meta::Dataset>> data;  //!< @brief

    /**
     * @brief
     * @param grp
     * @return
     */
    static boost::optional<genie::core::meta::DatasetGroup> decapsulate_dataset_group(
        genie::format::mgg::DatasetGroup* grp);

    /**
     * @brief
     * @param grp
     * @return
     */
    static std::map<uint8_t, genie::core::meta::Reference> decapsulate_references(
        genie::format::mgg::DatasetGroup* grp);

    /**
     * @brief
     * @param au
     * @return
     */
    static std::pair<genie::format::mgb::AccessUnit, boost::optional<genie::core::meta::AccessUnit>> decapsulate_AU(
        genie::format::mgg::AccessUnit& au);

    /**
     * @brief
     * @param dt
     * @param meta_group
     * @param meta_references
     * @param grp
     * @return
     */
    static std::pair<genie::format::mgb::MgbFile, genie::core::meta::Dataset> decapsulate_dataset(
        genie::format::mgg::Dataset& dt, boost::optional<genie::core::meta::DatasetGroup>& meta_group,
        std::map<uint8_t, genie::core::meta::Reference>& meta_references, genie::format::mgg::DatasetGroup* grp);

 public:
    /**
     * @brief
     * @param other
     */
    DecapsulatedDatasetGroup(DecapsulatedDatasetGroup&& other)  noexcept;

    /**
     * @brief
     * @param grp
     */
    explicit DecapsulatedDatasetGroup(genie::format::mgg::DatasetGroup* grp);

    /**
     * @brief
     * @return
     */
    uint64_t getID() const;

    /**
     * @brief
     * @return
     */
    bool hasMetaGroup() const;

    /**
     * @brief
     * @return
     */
    genie::core::meta::DatasetGroup& getMetaGroup();

    /**
     * @brief
     * @return
     */
    std::map<uint8_t, genie::core::meta::Reference>& getMetaReferences();

    /**
     * @brief
     * @return
     */
    std::map<uint64_t, std::pair<genie::format::mgb::MgbFile, genie::core::meta::Dataset>>& getData();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace encapsulator
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_DECAPSULATED_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
