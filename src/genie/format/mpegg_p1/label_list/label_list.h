#ifndef GENIE_PART1_LABEL_LIST_H
#define GENIE_PART1_LABEL_LIST_H

#include <cstdint>
#include <vector>

#include "label/label.h"
#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class LabelList{
   private:
    /**
     * ISO 23092-1 Section 6.5.1.5 table 14
     **/
    uint8_t dataset_group_ID;
    std::vector<Label> labels;

   public:

    LabelList();
    explicit LabelList(uint8_t _ds_group_ID);

    /**
     *
     * @param _ds_group_ID
     * @param labels
    **/
    LabelList(uint8_t _ds_group_ID, std::vector<Label>&& _labels);

//    /*
//     *
//     */
//    void addLabel(Label&& _label);
//
//    /*
//     *
//     */
//    void addLabels(std::vector<Label>& _labels);

    /**
     *
     * @param _labels
     */
    void setLabels(std::vector<Label>&& _labels);

    /*
     *
     */
    uint8_t getDatasetGroupID() const;

    /*
     * Implements num_labels
     */
    uint16_t getNumLabels() const;

    /*
     *
     */
    const std::vector<Label>& getLabels() const;

    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_LABEL_LIST_H
