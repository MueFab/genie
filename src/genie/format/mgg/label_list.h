/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_LABEL_LIST_H_
#define SRC_GENIE_FORMAT_MGG_LABEL_LIST_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/label.h"
#include "genie/util/bitreader.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief
 */
class LabelList : public GenInfo {
 private:
    uint8_t dataset_group_ID;   //!< @brief
    std::vector<Label> labels;  //!< @brief

 public:
    /**
     * @brief
     * @param groupID
     */
    void patchID(uint8_t groupID);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief
     * @param dataset
     * @return
     */
    std::vector<genie::core::meta::Label> decapsulate(uint16_t dataset);

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @param _ds_group_ID
     */
    explicit LabelList(uint8_t _ds_group_ID);

    /**
     * @brief
     * @param reader
     */
    explicit LabelList(util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<Label>& getLabels() const;

    /**
     * @brief
     * @param l
     */
    void addLabel(Label l);

    /**
     * @brief
     * @param bit_writer
     */
    void box_write(genie::util::BitWriter& bit_writer) const override;

    /**
     * @brief
     * @param output
     * @param depth
     * @param max_depth
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;

    /**
     * @brief
     * @param reader
     * @param in_offset
     */
    void read_box(util::BitReader& reader, bool in_offset);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_LABEL_LIST_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
