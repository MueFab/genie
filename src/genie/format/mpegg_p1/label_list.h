/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_LIST_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_LIST_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/format/mpegg_p1/label.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

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
     * @return
     */
    const std::string& getKey() const override;

    /**
     * @brief
     * @return
     */
    uint64_t getSize() const override;

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
    uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    const std::vector<Label>& getLabels() const;

    /**
     * @brief
     * @param l
     */
    void addLabel(Label l);

    /**
     * @brief
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_LIST_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
