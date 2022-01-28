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
#include "label.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class LabelList : public GenInfo {
 private:
    /**
     * ISO 23092-1 Section 6.5.1.5 table 14
     **/
    uint8_t dataset_group_ID;
    std::vector<Label> labels;

 public:

    const std::string& getKey() const override {
        static const std::string key = "labl";
        return key;
    }

    uint64_t getSize() const override {
        uint64_t labelSize = 0;
        for(const auto& l : labels) {
            labelSize += l.getSize();
        }
        return GenInfo::getSize() + sizeof(uint8_t) + sizeof(uint16_t) + labelSize;
    }

    /**
     *
     * @param _ds_group_ID
     */
    explicit LabelList(uint8_t _ds_group_ID);

    /**
     *
     * @param reader
     * @param length
     */
    explicit LabelList(util::BitReader& reader);

    /**
     *
     * @return
     */
    uint8_t getDatasetGroupID() const;
    /**
     *
     * @return
     */
    const std::vector<Label>& getLabels() const;

    void addLabel(Label l) {
        labels.emplace_back(std::move(l));
    }

    /**
     *
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
