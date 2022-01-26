/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_LIST_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_LIST_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/format/mpegg_p1/label_list/label/label.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class LabelList {
 private:
    /**
     * ISO 23092-1 Section 6.5.1.5 table 14
     **/
    uint8_t dataset_group_ID;
    std::vector<Label> labels;

 public:
    /**
     *
     */
    LabelList();
    /**
     *
     * @param _ds_group_ID
     */
    explicit LabelList(uint8_t _ds_group_ID);
    /**
     *
     * @param _ds_group_ID
     * @param _labels
     **/
    LabelList(uint8_t _ds_group_ID, std::vector<Label>&& _labels);
    /**
     *
     * @param reader
     * @param length
     */
    LabelList(util::BitReader& reader, size_t length);

    /**
     *
     * @param _dataset_group_ID
     */
    void setDatasetGroupId(uint8_t _dataset_group_ID);
    /**
     *
     * @return
     */
    uint8_t getDatasetGroupID() const;
    /**
     *
     * @param _labels
     */
    void setLabels(std::vector<Label>&& _labels);
    /**
     *
     * @return
     */
    const std::vector<Label>& getLabels() const;
    /**
     *
     * @return
     */
    uint16_t getNumLabels() const;

    /**
     *
     * @return
     */
    uint64_t getLength() const;
    /**
     *
     * @param bit_writer
     */
    void writeToFile(genie::util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_LIST_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
