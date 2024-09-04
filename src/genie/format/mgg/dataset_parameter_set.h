/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_PARAMETER_SET_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include <string>
#include "genie/core/constants.h"
#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mgg/dataset_parameterset/update_info.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief
 */
class DatasetParameterSet : public GenInfo {
 private:
    uint8_t dataset_group_id;                                      //!< @brief
    uint16_t dataset_id;                                           //!< @brief
    uint8_t parameter_set_ID;                                      //!< @brief
    uint8_t parent_parameter_set_ID;                               //!< @brief
    std::optional<dataset_parameterset::UpdateInfo> param_update;  //!< @brief
    genie::core::parameter::EncodingSet params;                    //!< @brief
    core::MPEGMinorVersion version;                                //!< @brief

 public:
    /**
     * @brief
     * @param _groupID
     * @param _setID
     */
    void patchID(uint8_t _groupID, uint16_t _setID);

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @param _dataset_group_id
     * @param _dataset_id
     * @param set
     * @param _version
     */
    DatasetParameterSet(uint8_t _dataset_group_id, uint16_t _dataset_id, genie::core::parameter::ParameterSet set,
                        core::MPEGMinorVersion _version);

    /**
     * @brief
     * @return
     */
    genie::core::parameter::ParameterSet descapsulate();

    /**
     * @brief
     * @param _dataset_group_id
     * @param _dataset_id
     * @param _parameter_set_ID
     * @param _parent_parameter_set_ID
     * @param ps
     * @param _version
     */
    DatasetParameterSet(uint8_t _dataset_group_id, uint16_t _dataset_id, uint8_t _parameter_set_ID,
                        uint8_t _parent_parameter_set_ID, genie::core::parameter::EncodingSet ps,
                        core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param reader
     * @param _version
     * @param parameters_update_flag
     */
    DatasetParameterSet(genie::util::BitReader& reader, core::MPEGMinorVersion _version, bool parameters_update_flag);

    /**
     * @brief
     * @param writer
     */
    void box_write(genie::util::BitWriter& writer) const override;

    /**
     * @brief
     * @param update
     */
    void addParameterUpdate(dataset_parameterset::UpdateInfo update);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint16_t getDatasetID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getParameterSetID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getParentParameterSetID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool hasParameterUpdate() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const dataset_parameterset::UpdateInfo& getParameterUpdate() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const genie::core::parameter::EncodingSet& getEncodingSet() const;

    /**
     * @brief
     * @return
     */
    genie::core::parameter::EncodingSet&& moveParameterSet();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
